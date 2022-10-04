#pragma once

namespace common
{


enum class errcode
{
	Success,
	NoMessage,
	RegisterWndClassFailed,
	CreateWindowFailed,
	CreateD3DDeviceFailed,
	CannotFindTheViewWindow,
	AttachProcessFailed,

	ReadProcessMemoryFailed,
	ReadProcessMemoryNotEnoughByte,

	WriteProcessMemoryFailed,
	WriteProcessMemoryNotEnoughByte,

	VirtualQueryFailed,

	EnumProcessModulesFailed,
	EnumProcesSymbolsFailed,

	TheImageFileFormatIsNotSupported,
	CannotParseImagePEHeader,
	CannotGetTargetProcesArchitecture,

	SymbolEngineFailedToFindRelativeVirtualAddress,

	AnalyzerFailedToReadMemory,
	AnalyzerFailedToDisassemble,
};

enum class errtype
{
	None,
	WinAPI,
	HRESULT,
};

inline std::map<errcode, std::wstring> rcErrorMessages = {
	{errcode::NoMessage                        , L"No message"},
	{errcode::RegisterWndClassFailed           , L"Failed to register window class"},
	{errcode::CreateWindowFailed               , L"Failed to create the window"},
	{errcode::CreateD3DDeviceFailed            , L"Failed to create D3D Device"},
	{errcode::CannotFindTheViewWindow          , L"Cannot find the view window"},
	{errcode::AttachProcessFailed              , L"Failed to attach to the target process"},
	{errcode::ReadProcessMemoryFailed          , L"Failed to read from the process memory"},
	{errcode::ReadProcessMemoryNotEnoughByte   , L"ReadProcessMemory didn't read enough bytes"},
	{errcode::WriteProcessMemoryFailed         , L"Failed to write to the process memory"},
	{errcode::WriteProcessMemoryNotEnoughByte  , L"WriteProcessMemory didn't write enough bytes"},
	{errcode::VirtualQueryFailed  , L"Failed to query virtual memory information"},
	{errcode::EnumProcessModulesFailed         , L"Failed to enumerate the process' modules"},
	{errcode::EnumProcesSymbolsFailed          , L"Failed to enumerate the process' symbols"},
	{errcode::TheImageFileFormatIsNotSupported , L"The image file format is not supported"},
	{errcode::CannotParseImagePEHeader         , L"Cannot parse the image PE Header"},
	{errcode::CannotGetTargetProcesArchitecture, L"Cannot get the target proces architecture (x86 or x64), please try again"},
	{errcode::SymbolEngineFailedToFindRelativeVirtualAddress, L"Symbol engine failed to find relative virtual address"},
	{errcode::AnalyzerFailedToReadMemory       , L"The analyzer had failed to read memory\nThis is probably because the VirtualQuery operation didn't succeed"},
	{errcode::AnalyzerFailedToDisassemble      , L"The analyzer had failed to disassemble instructions"},
};


class err
{
private:
	errcode code;
	errtype type;
	DWORD extra_code = 0;

public:

	err(
		const errcode code = errcode::NoMessage,
		errtype type = errtype::None,
		DWORD extra_code = 0
	)
		: code(code), type(type), extra_code(extra_code)
	{
	}

	inline std::wstring format()
	{
		auto find = rcErrorMessages.find(code);

		if (find == rcErrorMessages.end())
			return rcErrorMessages[errcode::NoMessage];

		return find->second;
	}

	void show(const std::wstring& title)
	{
		wchar_t message[1024];
		if (type == errtype::WinAPI && extra_code)
		{
			wchar_t* winMessage = nullptr;
			FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, extra_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&winMessage), 0, NULL);

			swprintf_s(message, L"%s\n\nWinAPI Error: %s\nWinAPI Error code: 0x%x", format().c_str(), winMessage, extra_code);
		}
		else if (type == errtype::HRESULT && extra_code)
		{
			_com_error err(static_cast<HRESULT>(extra_code));
			LPCTSTR errMsg = err.ErrorMessage();
			swprintf_s(message, L"%s\n\nHRESULT Error: %s\nHRESULT Error code: 0x%x", format().c_str(), errMsg, extra_code);
		}
		else
		{
			swprintf_s(message, L"%s", format().c_str());
		}

		MessageBoxW(NULL, message, title.c_str(), MB_ICONERROR);
	}

	inline void show()
	{
		return show(L"Error");
	}
};



}
