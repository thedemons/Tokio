#include "stdafx.h"
#include "ViewDecompiler.h"
#include "Engine/Engine.h"

ViewDecompiler::ViewDecompiler()
{
	m_title = MainView::GetViewTitle<ViewDecompiler>(ICON_MEMORY_SCAN u8" Decompiler");

	auto lang = Widgets::TextEditor::LanguageDefinition::CPlusPlus();

	// set your own known preprocessor symbols...
	static const char* ppnames[] = { "NULL", "PM_REMOVE",
		"ZeroMemory", "DXGI_SWAP_EFFECT_DISCARD", "D3D_FEATURE_LEVEL", "D3D_DRIVER_TYPE_HARDWARE", "WINAPI","D3D11_SDK_VERSION", "assert" };
	// ... and their corresponding values
	static const char* ppvalues[] = {
		"#define NULL ((void*)0)",
		"#define PM_REMOVE (0x0001)",
		"Microsoft's own memory zapper function\n(which is a macro actually)\nvoid ZeroMemory(\n\t[in] PVOID  Destination,\n\t[in] SIZE_T Length\n); ",
		"enum DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD = 0",
		"enum D3D_FEATURE_LEVEL",
		"enum D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE  = ( D3D_DRIVER_TYPE_UNKNOWN + 1 )",
		"#define WINAPI __stdcall",
		"#define D3D11_SDK_VERSION (7)",
		" #define assert(expression) (void)(                                                  \n"
		"    (!!(expression)) ||                                                              \n"
		"    (_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \n"
		" )"
	};

	for (int i = 0; i < sizeof(ppnames) / sizeof(ppnames[0]); ++i)
	{
		Widgets::TextEditor::Identifier id;
		id.mDeclaration = ppvalues[i];
		lang.mPreprocIdentifiers.insert(std::make_pair(std::string(ppnames[i]), id));
	}

	// set your own identifiers
	static const char* identifiers[] = {
		"HWND", "HRESULT", "LPRESULT","D3D11_RENDER_TARGET_VIEW_DESC", "DXGI_SWAP_CHAIN_DESC","MSG","LRESULT","WPARAM", "LPARAM","UINT","LPVOID",
		"ID3D11Device", "ID3D11DeviceContext", "ID3D11Buffer", "ID3D11Buffer", "ID3D10Blob", "ID3D11VertexShader", "ID3D11InputLayout", "ID3D11Buffer",
		"ID3D10Blob", "ID3D11PixelShader", "ID3D11SamplerState", "ID3D11ShaderResourceView", "ID3D11RasterizerState", "ID3D11BlendState", "ID3D11DepthStencilState",
		"IDXGISwapChain", "ID3D11RenderTargetView", "ID3D11Texture2D", "TextEditor",
		"int128_t", "int64_t", "int32_t", "int16_t", "int8_t", "int1_t",
		"uint128_t", "uint64_t", "uint32_t", "uint16_t", "uint8_t", "uint1_t", 
	};
	static const char* idecls[] =
	{
		"typedef HWND_* HWND", "typedef long HRESULT", "typedef long* LPRESULT", "struct D3D11_RENDER_TARGET_VIEW_DESC", "struct DXGI_SWAP_CHAIN_DESC",
		"typedef tagMSG MSG\n * Message structure","typedef LONG_PTR LRESULT","WPARAM", "LPARAM","UINT","LPVOID",
		"ID3D11Device", "ID3D11DeviceContext", "ID3D11Buffer", "ID3D11Buffer", "ID3D10Blob", "ID3D11VertexShader", "ID3D11InputLayout", "ID3D11Buffer",
		"ID3D10Blob", "ID3D11PixelShader", "ID3D11SamplerState", "ID3D11ShaderResourceView", "ID3D11RasterizerState", "ID3D11BlendState", "ID3D11DepthStencilState",
		"IDXGISwapChain", "ID3D11RenderTargetView", "ID3D11Texture2D", "class TextEditor",
		"typedef int128 int128_t", "typedef long long int64_t",  "typedef long int32_t", "typedef short int16_t", "typedef char int8_t", "typedef bool int1_t",
		"typedef unsigned int128 int128_t", "typedef unsigned long long int64_t",  "typedef unsigned long int32_t", "typedef unsigned short int16_t", "typedef unsigned char int8_t", "typedef bool int1_t",
	};

	for (int i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]); ++i)
	{
		Widgets::TextEditor::Identifier id;
		id.mDeclaration = std::string(idecls[i]);
		lang.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
	}
	m_textEditor.SetLanguageDefinition(lang);

	Widgets::TextEditor::Palette palette = {
		0xffffffff, // Default
		0xffD69C56, // Keyword
		0xffA8CEB5, // Number
		0xff809DD6, // String
		0xffBBC9E8, // CharLiteral
		0xffB4B4B4, // Punctuation
		0xff569CD6, // preprocessor
		0xffDADADA, // Identifier
		0xffA7C94E, // KnownIdentifier
		0xff0000ff, // PreprocIdentifier
		0xff4AA657, // Comment
		0xff4AA657, // MultiLineComment
		0xff1E1E1E, // Background
		0xffDCDCDC, // Cursor
		0xff784F26, // Selection
		0xff0000ff, // ErrorMarker
		0xff0000ff, // breakpoint
		0xffAF912B, // line number
		0x00000000, // CurrentLineFill
		0x00000000, // CurrentLineFillInactive
		0xff464646, // CurrentLineEdge
	};

	m_textEditor.SetPalette(palette);

	//m_textEditor.setta
	//editor.SetPalette(TextEditor::GetLightPalette());

	//// error markers
	//Widgets::TextEditor::ErrorMarkers markers;
	//markers.insert(std::make_pair<int, std::string>(6, "Example error here:\nInclude file not found: \"TextEditor.h\""));
	//markers.insert(std::make_pair<int, std::string>(41, "Another example error"));
	//m_textEditor.SetErrorMarkers(markers);
}

void ViewDecompiler::Render(bool& bOpen)
{
	if (!bOpen) return;

	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 0.f, 0.f });
	ImGui::Begin(Title().c_str(), &bOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PopStyleVar();

	ImGui::PushFont(MainApplication::FontMonoRegular);
	m_textEditor.Render("TextEditor");
	ImGui::PopFont();

	ImGui::End();

	//ImGui::FocusWindow()


}

void ViewDecompiler::Decompile(POINTER address, size_t size)
{
	std::string decompiled = Engine::Decompile(address, size);
	m_textEditor.SetText(decompiled);
	//ImGui::FocusWindow(ImGui::FindWindowByName(m_title.c_str()));

}