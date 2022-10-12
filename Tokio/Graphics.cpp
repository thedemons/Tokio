/*
 * Project Tokio
 * Author: thedemons
 * Github: thedemons/Tokio
 * Made:   With love
 *
 * License: MIT License
 *
 * Copyright(c) 2022 thedemons
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "stdafx.h"
#include "Graphics.h"
#include "imgui.hpp"
#include "Settings.h"
#include "GUI\Icons.hpp"

#pragma warning (push)
#pragma warning (disable : 4005)
#include <d3d11.h>
#pragma warning (pop)


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API void ImGui_ImplWin32_EnableAlphaCompositing(void* hwnd);

namespace Graphics
{

HWND hWnd = nullptr;
WNDCLASSEX wndClass;

ID3D11Device* g_pd3dDevice = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
ID3D11RenderTargetView* g_pMainRenderTargetView = nullptr;

bool isNeedReloadFont = false;

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


void InitWindow() EXCEPT
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

	ATOM wndCUnique = ::RegisterClassExW(&wndClass);
	if (!wndCUnique)
	{
		throw Tokio::Exception(Tokio::Exception::Type::WinAPI, "Register class failed");
	}

	hWnd = ::CreateWindowExW(
		NULL, wndClass.lpszClassName,
		L"Tokio",
		WS_OVERLAPPEDWINDOW,
		100, 100, 1280, 800,
		NULL, NULL, wndClass.hInstance, NULL
	);

	if (!hWnd)
	{
		throw Tokio::Exception(Tokio::Exception::Type::WinAPI, "Failed to create window");
	}
}

void CreateRenderTarget() EXCEPT
{
	// create render target
	ID3D11Texture2D* pBackBuffer;

	if (g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)) != S_OK)
	{
		throw Tokio::Exception("Failed get DirectX back buffer");
	}

	if (g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pMainRenderTargetView) != S_OK)
	{
		throw Tokio::Exception("Failed to create render target");
	}

	pBackBuffer->Release();
}

void InitDevice() EXCEPT
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

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
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
	);


	if (hr != S_OK)
	{
		throw Tokio::Exception("Failed to create DirectX device");
	}

	CreateRenderTarget();
}

void InitImgui() EXCEPT
{
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
}

ImFont* AddFontFromFile(const char* ttf, float size)
{
	static auto fontAtlas = ImGui::GetIO().Fonts;
	static auto glyphRange = fontAtlas->GetGlyphRangesVietnamese();
	auto font = fontAtlas->AddFontFromFileTTF(ttf, size, 0, glyphRange);

	static const ImWchar rangesAwesome[] = { ICON_MIN_FA, ICON_MAX_FA, ICON_DUO_OFFSET + ICON_MIN_FA, ICON_DUO_OFFSET + ICON_MAX_FA, 0 };
	ImFontConfig configAwesome;
	configAwesome.MergeMode = true;
	//configAwesome.FontDataOwnedByAtlas = false; // don't free memory
	configAwesome.OversampleH = configAwesome.OversampleV = 1;
	//fontAtlas->AddFontFromMemoryTTF((void*)fa_solid_900, sizeof(fa_solid_900), 12.f, &configAwesome, rangesAwesome);
	fontAtlas->AddFontFromFileTTF("Fonts\\fa-duotone-900.ttf", roundf(size*0.9f), &configAwesome, rangesAwesome);

	return font;
}

void BuildFont()
{
	auto& io = ImGui::GetIO();
	io.Fonts->Build();

	int tex_width, tex_height;
	ImU32* tex_pixels = nullptr;
	io.Fonts->GetTexDataAsRGBA32(reinterpret_cast<unsigned char**>(&tex_pixels), &tex_width, &tex_height);

	struct IconColorData
	{
		const char* icon;
		ImU32 color_bot;
		ImU32 color_top;
	};

	const auto& col_bot = Settings::theme.iconColor1;
	const auto& col_top = Settings::theme.iconColor2;
	ImFont* fontList[] = { FontRegular, FontBold, FontMono, FontMonoBold };
	IconColorData glyphCol[] = {
		{ICON_TOKIO_SAVE              , col_bot.save              , col_top.save              },
		{ICON_TOKIO_COPY              , col_bot.copy              , col_top.copy              },
		{ICON_TOKIO_GOTO_ADDRESS      , col_bot.goto_address      , col_top.goto_address      },
		{ICON_TOKIO_FOLLOW_ADDRESS    , col_bot.follow_address    , col_top.follow_address    },
		{ICON_TOKIO_FIND_REFERENCES   , col_bot.find_references   , col_top.find_references   },
		{ICON_TOKIO_SWITCH_MODE       , col_bot.switch_mode       , col_top.switch_mode       },
		{ICON_TOKIO_OPEN_FILE_LOCATION, col_bot.open_file_location, col_top.open_file_location},
		{ICON_TOKIO_FREEZE            , col_bot.freeze            , col_top.freeze            },
		{ICON_TOKIO_POWER_OFF         , col_bot.power_off         , col_top.power_off         },
		{ICON_TOKIO_REFRESH           , col_bot.refresh           , col_top.refresh           },

		{ICON_TOKIO_ATTACH_PROC       , col_bot.attach_proc       , col_top.attach_proc       },
		{ICON_TOKIO_SETTINGS          , col_bot.settings          , col_top.settings          },
		{ICON_TOKIO_SYMBOL_LIST       , col_bot.symbol_list       , col_top.symbol_list       },
		{ICON_TOKIO_MEMORY_VIEW       , col_bot.memory_view       , col_top.memory_view       },
		{ICON_TOKIO_MEMORY_SCAN       , col_bot.memory_scan       , col_top.memory_scan       },
		{ICON_TOKIO_WATCH_LIST        , col_bot.watch_list        , col_top.watch_list        },
		{ICON_TOKIO_DISASSEMBLER      , col_bot.disassembler      , col_top.disassembler      },
		{ICON_TOKIO_PSEUDO_CODE       , col_bot.pseudo_code       , col_top.pseudo_code       },
	};


	for (ImFont* font : fontList)
	{
		for (auto& [utf8, color_bot, color_top] : glyphCol)
		{
			unsigned int c = 0x00, c2 = 0x00;
			ImTextCharFromUtf8(&c, utf8, utf8 + 0x4);
			c2 = ICON_DUO_OFFSET + c;

			ImFontGlyph* bot_glyph = (ImFontGlyph*)font->FindGlyph(c);
			ImFontGlyph* top_glyph = (ImFontGlyph*)font->FindGlyph(c2);

			// Fill the custom rectangle with red pixels (in reality you would draw/copy your bitmap data here!)

			size_t bot_x = size_t(roundf( bot_glyph->U0 * tex_width));
			size_t bot_y = size_t(roundf( bot_glyph->V0 * tex_height));
			size_t bot_w = size_t(roundf((bot_glyph->U1 - bot_glyph->U0) * tex_width));
			size_t bot_h = size_t(roundf((bot_glyph->V1 - bot_glyph->V0) * tex_height));

			size_t top_x = size_t(roundf( top_glyph->U0 * tex_width));
			size_t top_y = size_t(roundf( top_glyph->V0 * tex_height));
			size_t top_w = size_t(roundf((top_glyph->U1 - top_glyph->U0) * tex_width));
			size_t top_h = size_t(roundf((top_glyph->V1 - top_glyph->V0) * tex_height));


			bot_glyph->Colored = true;
			top_glyph->Colored = true;

			for (size_t y = 0; y < bot_h; y++)
			{
				ImU32* row_bmp = tex_pixels + (bot_y + y) * tex_width;
				for (size_t x = 0; x < bot_w; x++)
				{
					ImU32* pixel_bmp = row_bmp + (bot_x + x);

					ImU32 alpha = ((*pixel_bmp >> 24) & 0xff) << 24;
					ImU32 newCol = alpha | color_bot;
					*pixel_bmp = newCol;
				}
			}

			for (size_t y = 0; y < top_h; y++)
			{
				ImU32* row_bmp = tex_pixels + (top_y + y) * tex_width;
				for (size_t x = 0; x < top_w; x++)
				{
					ImU32* pixel_bmp = row_bmp + (top_x + x);
					ImU32 alpha = ((*pixel_bmp >> 24) & 0xff) << 24;
					ImU32 newCol = alpha | color_top;
					*pixel_bmp = newCol;
				}
			}

		}
	}
}

void LoadFont() noexcept
{
	FontRegular = AddFontFromFile("Fonts\\NotoSans-Regular.ttf", Settings::theme.font.fontSize);
	FontBold = AddFontFromFile("Fonts\\NotoSans-Bold.ttf", Settings::theme.font.fontSize);

	FontMono = AddFontFromFile("Fonts\\CascadiaMonoPL-Regular.ttf", Settings::theme.font.fontMonoSize);
	FontMonoBold = AddFontFromFile("Fonts\\CascadiaMonoPL-Bold.ttf", Settings::theme.font.fontMonoSize);

	BuildFont();
}

void ReloadFont() noexcept
{
	isNeedReloadFont = true;
}

void ShowWindow() noexcept
{
	::ShowWindow(hWnd, SW_MAXIMIZE);
	::UpdateWindow(hWnd);
}

void CleanupRenderTarget()
{
	if (g_pMainRenderTargetView) { g_pMainRenderTargetView->Release(); g_pMainRenderTargetView = NULL; }
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}


LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
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
				::SetWindowPos(hwnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
			break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

bool BeginRender() noexcept
{
	static const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 0.f };

	MSG wndMessage;
	while (::PeekMessage(&wndMessage, NULL, 0U, 0U, PM_REMOVE))
	{
		::TranslateMessage(&wndMessage);
		::DispatchMessage(&wndMessage);
		if (wndMessage.message == WM_QUIT) return false;
	}

	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_pMainRenderTargetView, NULL);
	g_pd3dDeviceContext->ClearRenderTargetView(g_pMainRenderTargetView, clear_color_with_alpha);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	return true;
}

void EndRender() noexcept
{
	static const auto& io = ImGui::GetIO();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	if (Settings::general.bEnableVsync)
		g_pSwapChain->Present(1, 0); // Present with vsync
	else
		g_pSwapChain->Present(0, 0); // Present without vsync

	if (isNeedReloadFont)
	{
		isNeedReloadFont = false;

		io.Fonts->ClearTexData();
		BuildFont();
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}
}

void CleanUp() noexcept
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hWnd);
	::UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);
}

WindowHandle GetWindow() noexcept
{
	return static_cast<WindowHandle>(hWnd);
}

ImTextureID TextureFromBitmap(const void* pBitmap, unsigned int width, unsigned int height) noexcept NULLABLE
{
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = pBitmap;
	subResource.SysMemPitch = width * 4;
	subResource.SysMemSlicePitch = 0;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = 0;

	ID3D11Texture2D* pTexture = nullptr;
	HRESULT hr = g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);
	if (hr != S_OK) return nullptr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* pResource = nullptr;
	hr = g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &pResource);

	if (hr != S_OK)
	{
		pTexture->Release();
		if (hr != S_OK) return nullptr;
	}

	return pResource;
}


}