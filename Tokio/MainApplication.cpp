#include "stdafx.h"
#include "MainApplication.h"
#include "Resources/FontAwesome.h"
#include "Resources/FontAwesomeImpl.h"
//#include <dwmapi.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API void ImGui_ImplWin32_EnableAlphaCompositing(void* hwnd);
namespace MainApplication
{
PRenderCallBack RenderCallback;

void CreateRenderTarget();
void CleanupRenderTarget();
void CleanupDeviceD3D();
bool CreateDeviceD3D();
ImFont* AddFontFromFile(const char* ttf, float size);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



auto Init() noexcept -> cpp::result<void, common::err>
{
    wndClass.cbSize = sizeof(WNDCLASSEXW);
    wndClass.style = CS_CLASSDC;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = GetModuleHandle(0);
    wndClass.hIcon = nullptr;
    wndClass.hCursor = nullptr;
    wndClass.hbrBackground = nullptr;
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = L"Tokio";

    SetLastError(0);

    ATOM wndCUnique = ::RegisterClassExW(&wndClass);
    WINAPI_FAILIFN(wndCUnique, RegisterWndClassFailed);


    hWnd = ::CreateWindowExW(
        NULL, wndClass.lpszClassName,
        L"Tokio",
        WS_OVERLAPPEDWINDOW,
        100, 100, 1280, 800,
        NULL, NULL, wndClass.hInstance, NULL
    );

    //MARGINS margins{ -1, -1, -1, -1 };
    //DwmExtendFrameIntoClientArea(hWnd, &margins);

    WINAPI_FAILIFN(hWnd, RegisterWndClassFailed);
    RESULT_FAILIFN(CreateDeviceD3D(), CreateD3DDeviceFailed);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowMinSize = { 100.f, 50.f };

    ImGui::StyleColorsDark();
    
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    //ImGui_ImplWin32_EnableAlphaCompositing(hWnd);


    // TODO: Move fonts to resources
    FontRegular           = AddFontFromFile("Fonts\\NotoSans-Regular.ttf"          , 17.f);
    //FontThin              = AddFontFromFile("Fonts\\NotoSans-Light.ttf"            , 17.f);
    FontBold              = AddFontFromFile("Fonts\\NotoSans-Bold.ttf"             , 17.f);

    //FontItalicThin        = AddFontFromFile("Fonts\\NotoSans-LightItalic.ttf"      , 17.f);
    //FontItalicBold        = AddFontFromFile("Fonts\\NotoSans-BoldItalic.ttf"       , 17.f);
    //FontItalicRegular     = AddFontFromFile("Fonts\\NotoSans-Italic.ttf"           , 17.f);

    //FontMonoThin          = AddFontFromFile("Fonts\\CascadiaMonoPL-Light.ttf"      , 15.f);
    FontMonoBold          = AddFontFromFile("Fonts\\CascadiaMonoPL-Bold.ttf"       , 14.f);
    FontMonoBigBold          = AddFontFromFile("Fonts\\CascadiaMonoPL-Bold.ttf"    , 15.f);
    FontMonoRegular       = AddFontFromFile("Fonts\\CascadiaMonoPL-Regular.ttf"    , 14.f);

    FontMonoItalicThin    = AddFontFromFile("Fonts\\CascadiaMonoPL-LightItalic.ttf", 14.f);
    //FontMonoItalicBold    = AddFontFromFile("Fonts\\CascadiaMonoPL-BoldItalic.ttf" , 15.f);
    //FontMonoItalicRegular = AddFontFromFile("Fonts\\CascadiaMonoPL-Italic.ttf"     , 15.f);


    ::ShowWindow(hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hWnd);

    return {};
}


void FontMergeIcon()
{
    auto fontAtlas = ImGui::GetIO().Fonts;

    // need to be static (or alive)
    static const ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };
    ImFontConfig configEmoji;
    configEmoji.MergeMode = true;
    configEmoji.FontBuilderFlags |= 1 << 8; // ImGuiFreeTypeBuilderFlags_LoadColor
    configEmoji.FontDataOwnedByAtlas = true; // free memory on release
    configEmoji.OversampleH = configEmoji.OversampleV = 1;
    fontAtlas->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguiemj.ttf", 12.0f, &configEmoji, ranges);

    static const ImWchar rangesAwesome[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig configAwesome;
    configAwesome.MergeMode = true;
    configAwesome.FontDataOwnedByAtlas = false; // don't free memory
    configAwesome.OversampleH = configAwesome.OversampleV = 1;
    fontAtlas->AddFontFromMemoryTTF((void*)fa_solid_900, sizeof(fa_solid_900), 12.f, &configAwesome, rangesAwesome);

}

ImFont* AddFontFromFile(const char* ttf, float size)
{
    static auto fontAtlas = ImGui::GetIO().Fonts;
    static auto glyphRange = fontAtlas->GetGlyphRangesVietnamese();
    auto font = fontAtlas->AddFontFromFileTTF(ttf, size, 0, glyphRange);

    FontMergeIcon();
    return font;
}

bool CreateDeviceD3D()
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1; 
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
#ifdef DX11_ENABLE_DEBUG_LAYER
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // DX11_ENABLE_DEBUG_LAYER

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (
        D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            createDeviceFlags,
            featureLevelArray,
            2,
            D3D11_SDK_VERSION,
            &sd,
            &g_pSwapChain,
            &g_pd3dDevice,
            &featureLevel,
            &g_pd3dDeviceContext
        ) != S_OK
    )
    return false;

    CreateRenderTarget();

#ifdef DX11_ENABLE_DEBUG_LAYER

    // Thanks to the author of the blog
    // https://walbourn.github.io/direct3d-sdk-debug-layer-tricks/

    if (SUCCEEDED(g_pd3dDevice->QueryInterface(IID_ID3D11Debug, (void**)&g_pd3dDebug)))
    {
        g_pd3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

        if (SUCCEEDED(g_pd3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&g_pd3dInfoQueue)))
        {
            //g_pd3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_INFO, true);
            //g_pd3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, true);
            //g_pd3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
            g_pd3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);

            D3D11_MESSAGE_ID hide[] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // Add more message IDs here as needed
            };

            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            g_pd3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif // DX11_ENABLE_DEBUG_LAYER

    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pMainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_pMainRenderTargetView) { g_pMainRenderTargetView->Release(); g_pMainRenderTargetView = NULL; }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
        case WM_SIZE:
            if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
            {
                CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        case WM_DPICHANGED:
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
            {
                //const int dpi = HIWORD(wParam);
                //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
                const RECT* suggested_rect = (RECT*)lParam;
                ::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
            }
            break;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}


void SetRenderCallback(PRenderCallBack callback)
{
    RenderCallback = callback;
}

void StartLoop()
{
    bool done = false;
    const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 0.f };

    while (true)
    {
        MSG wndMessage;
        while (::PeekMessage(&wndMessage, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&wndMessage);
            ::DispatchMessage(&wndMessage);
            if (wndMessage.message == WM_QUIT) done = true;
        }
        if (done) break;

        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_pMainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_pMainRenderTargetView, clear_color_with_alpha);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (RenderCallback) RenderCallback();

        static char fps_info[1024];
        sprintf_s(fps_info, "FPS: %.2f", ImGui::GetIO().Framerate);

        ImGui::GetForegroundDrawList()->AddText({ ImGui::GetMainViewport()->Size.x - 100.f, 5.f}, 0xffffffff, fps_info);


        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hWnd);
    ::UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);

}

#ifdef DX11_ENABLE_DEBUG_LAYER
void D3DDebugPrintMsg()
{
    static std::map<D3D11_MESSAGE_CATEGORY, const char*> fmtCategory = {
        {D3D11_MESSAGE_CATEGORY_APPLICATION_DEFINED,    "APPLICATION_DEFINED"  },
        {D3D11_MESSAGE_CATEGORY_MISCELLANEOUS,          "MISCELLANEOUS"        },
        {D3D11_MESSAGE_CATEGORY_INITIALIZATION,         "INITIALIZATION"       },
        {D3D11_MESSAGE_CATEGORY_CLEANUP,                "CLEANUP"              },
        {D3D11_MESSAGE_CATEGORY_COMPILATION,            "COMPILATION"          },
        {D3D11_MESSAGE_CATEGORY_STATE_CREATION,         "STATE_CREATION"       },
        {D3D11_MESSAGE_CATEGORY_STATE_SETTING,          "STATE_SETTING"        },
        {D3D11_MESSAGE_CATEGORY_STATE_GETTING,          "STATE_GETTING"        },
        {D3D11_MESSAGE_CATEGORY_RESOURCE_MANIPULATION,  "RESOURCE_MANIPULATION"},
        {D3D11_MESSAGE_CATEGORY_EXECUTION,              "EXECUTION"            },
                                                                               };
    static std::map<D3D11_MESSAGE_SEVERITY, const char*> fmtSeverity = {
        {D3D11_MESSAGE_SEVERITY_CORRUPTION,             "CORRUPTION"           },
        {D3D11_MESSAGE_SEVERITY_ERROR,                  "ERROR"                },
        {D3D11_MESSAGE_SEVERITY_WARNING,                "WARNING"              },
        {D3D11_MESSAGE_SEVERITY_INFO,                   "INFO"                 },
    };

    // Thanks to the author of the code
    // https://stackoverflow.com/a/57362700/13253010
    UINT64 message_count = g_pd3dInfoQueue->GetNumStoredMessages();

    for (UINT64 i = 0; i < message_count; i++)
    {
        SIZE_T message_size = 0;
        g_pd3dInfoQueue->GetMessage(i, nullptr, &message_size); //get the size of the message

        D3D11_MESSAGE* message = (D3D11_MESSAGE*)malloc(message_size); //allocate enough space
        g_pd3dInfoQueue->GetMessage(i, message, &message_size); //get the actual message
        if (message)
        {
            printf(
                "ID: 0x%x Severity: %s Category: %s\n%.*s\n\n",
                message->ID,
                fmtSeverity[message->Severity],
                fmtCategory[message->Category],
                static_cast<int>(message->DescriptionByteLength),
                message->pDescription
            );

            free(message);
        }
    }

     g_pd3dInfoQueue->ClearStoredMessages();
}
#endif

}