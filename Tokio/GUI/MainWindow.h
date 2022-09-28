#pragma once
#ifndef TOKIO_MAINWINDOW_H
#define TOKIO_MAINWINDOW_H

#ifndef IMGUI_API
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/Impl/imgui_impl_dx11.h"
#include "ImGui/Impl/imgui_impl_win32.h"
#endif // IMGUI_API

#include <D3D11.h>
#include <D3DX11.h>

#ifdef _DEBUG
#define DX11_ENABLE_DEBUG_LAYER

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

#define DX_DEBUG(statement) \
if ((statement) != S_OK) {\
	MainWindow::D3DDebugPrintMsg();\
}

#define DX_THROWIFN(statement) \
if ((statement) != S_OK) {\
	MainWindow::D3DDebugPrintMsg();\
	throw;\
}

#define DX_FAILIFN(statement, code) \
if ((statement) != S_OK) {\
	MainWindow::D3DDebugPrintMsg();\
	return cpp::fail(common::err(common::errcode::code)); \
}


#else 
#define DX_DEBUG(statement) statement
#define DX_THROWIFN(statement) if ((statement) != S_OK) throw
#define DX_FAILIFN(statement, code) if ((statement) != S_OK) return cpp::fail(common::err(common::errcode::code))
#endif

namespace MainWindow
{

typedef void(* PRenderCallBack)();


inline WNDCLASSEX wndClass;
inline HWND hWnd;

inline ID3D11Device* g_pd3dDevice = nullptr;
inline IDXGISwapChain* g_pSwapChain = nullptr;
inline ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
inline ID3D11RenderTargetView* g_pMainRenderTargetView = nullptr;


auto Init() noexcept -> cpp::result<void, common::err>;
void SetRenderCallback(PRenderCallBack callback);
void StartLoop();

#ifdef DX11_ENABLE_DEBUG_LAYER
inline ID3D11Debug* g_pd3dDebug = nullptr;
inline ID3D11InfoQueue* g_pd3dInfoQueue = nullptr;
void D3DDebugPrintMsg();
#endif
}

#endif