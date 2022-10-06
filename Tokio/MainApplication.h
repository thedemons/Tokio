#pragma once
#ifndef TOKIO_MAINWINDOW_H
#define TOKIO_MAINWINDOW_H

#include "Common/Exception.h"

#include "imgui.hpp"

// suppress macro redefinition warning from d3d11.h
#pragma warning (push)
#pragma warning (disable : 4005)
#include <d3d11.h>
#pragma warning (pop)

#ifdef _DEBUG
#define DX11_ENABLE_DEBUG_LAYER

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

#define DX_DEBUG(statement) \
if ((statement) != S_OK) {\
	MainApplication::D3DDebugPrintMsg();\
}

#define DX_THROWIFN(statement) \
if ((statement) != S_OK) {\
	MainApplication::D3DDebugPrintMsg();\
	throw;\
}

#define DX_FAILIFN(statement, code) \
if ((statement) != S_OK) {\
	MainApplication::D3DDebugPrintMsg();\
	return cpp::fail(common::err(common::errcode::code)); \
}
#else 
#define DX_DEBUG(statement) statement
#define DX_THROWIFN(statement) if ((statement) != S_OK) throw
#define DX_FAILIFN(statement, code) if ((statement) != S_OK) return cpp::fail(common::err(common::errcode::code))
#endif // !_DEBUG


namespace MainApplication
{

typedef void(* PRenderCallBack)();

inline ID3D11Device* g_pd3dDevice = nullptr;
inline IDXGISwapChain* g_pSwapChain = nullptr;
inline ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
inline ID3D11RenderTargetView* g_pMainRenderTargetView = nullptr;

inline ImFont* FontThin              = nullptr;
inline ImFont* FontRegular           = nullptr;
inline ImFont* FontBold              = nullptr;

inline ImFont* FontItalicThin        = nullptr;
inline ImFont* FontItalicRegular     = nullptr;
inline ImFont* FontItalicBold        = nullptr;

inline ImFont* FontMonoThin          = nullptr;
inline ImFont* FontMonoRegular       = nullptr;
inline ImFont* FontMonoBold          = nullptr;
inline ImFont* FontMonoBigBold          = nullptr;

inline ImFont* FontMonoItalicThin    = nullptr;
inline ImFont* FontMonoItalicRegular = nullptr;
inline ImFont* FontMonoItalicBold    = nullptr;

void Init() EXCEPT;
void SetRenderCallback(PRenderCallBack callback);
void StartLoop();

#ifdef DX11_ENABLE_DEBUG_LAYER
inline ID3D11Debug* g_pd3dDebug = nullptr;
inline ID3D11InfoQueue* g_pd3dInfoQueue = nullptr;
void D3DDebugPrintMsg();
#endif
}

#endif // !TOKIO_MAINWINDOW_H