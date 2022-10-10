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
#include "Settings.h"
#include "imgui.hpp"

#include "Resources/FontAwesome.h"
#include "Resources/FontAwesomeImpl.h"

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
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
}

ImFont* AddFontFromFile(const char* ttf, float size)
{
	static auto fontAtlas = ImGui::GetIO().Fonts;
	static auto glyphRange = fontAtlas->GetGlyphRangesVietnamese();

	ImFont* font = fontAtlas->AddFontFromFileTTF(ttf, size, 0, glyphRange);

	static const ImWchar rangesAwesome[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig configAwesome;
	configAwesome.MergeMode = true;
	configAwesome.FontDataOwnedByAtlas = false; // don't free memory
	configAwesome.OversampleH = configAwesome.OversampleV = 1;
	fontAtlas->AddFontFromFileTTF("Fonts\\fa-solid-900.ttf", 14.0f, &configAwesome, rangesAwesome);

	return font;
}

void LoadFont() noexcept
{

	FontRegular = AddFontFromFile("Fonts\\NotoSans-Regular.ttf", 17.f);
	FontBold = AddFontFromFile("Fonts\\NotoSans-Bold.ttf", 17.f);

	FontMono = AddFontFromFile("Fonts\\CascadiaMonoPL-Regular.ttf", 14.f);
	FontMonoBold = AddFontFromFile("Fonts\\CascadiaMonoPL-Bold.ttf", 14.f);

	ImGui::GetIO().Fonts->Build();
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
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (Settings::general.bEnableVsync)
		g_pSwapChain->Present(1, 0); // Present with vsync
	else
		g_pSwapChain->Present(0, 0); // Present without vsync
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