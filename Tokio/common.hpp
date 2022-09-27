#pragma once

namespace common
{


enum class errcode
{
	NoMessage,
	RegisterWndClassFailed,
	CreateWindowFailed,
	CreateD3DDeviceFailed,
	CannotFindTheViewWindow,
};

inline std::unordered_map<errcode, std::wstring> rcErrorMessages = {
	{errcode::NoMessage, L"No message"},
	{errcode::RegisterWndClassFailed, L"Failed to register window class"},
	{errcode::CreateWindowFailed, L"Failed to create the window"},
	{errcode::CreateD3DDeviceFailed, L"Failed to create D3D Device"},
	{errcode::CannotFindTheViewWindow, L"Cannot find the view window"},
};


class err
{
private:
	errcode code;
	bool isWinApiErr;
	DWORD winApiCode = 0;

public:

	err(const errcode code = errcode::NoMessage, bool isWinApiErr = false)
		: code(code), isWinApiErr(isWinApiErr)
	{
		if (isWinApiErr) winApiCode = GetLastError();
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
		if (isWinApiErr && winApiCode)
		{
			wchar_t* winMessage = nullptr;
			size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, winApiCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&winMessage), 0, NULL);

			swprintf_s(message, L"%s\n\nWinAPI Error: %s\nWinAPI Error code: 0x%x", format().c_str(), winMessage, winApiCode);
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
