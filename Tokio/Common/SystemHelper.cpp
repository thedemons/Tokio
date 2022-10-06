#pragma once
#include "stdafx.h"
#include "SystemHelper.h"
#include "StringHelper.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#include <WinUser.h>
#include <ShlObj.h>

namespace Tokio
{



_NODISCARD std::map<PID, ProcessEntry> GetAllProcess() EXCEPT
{
	std::map<PID, ProcessEntry> result;

	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (snapshot == INVALID_HANDLE_VALUE)
	{
		static const std::string except_msg("GetAllProcess CreateToolhelp32Snapshot failed");
		throw Exception(except_msg, Exception::Type::WinAPI);
	}

	if (!Process32FirstW(snapshot, &entry))
	{
		static const std::string except_msg("GetAllProcess Process32FirstW failed");
		throw Exception(except_msg, Exception::Type::WinAPI);
	}

	while (Process32NextW(snapshot, &entry) == TRUE)
	{
		ProcessEntry& data = result[static_cast<PID>(entry.th32ProcessID)];
		data.pid       = entry.th32ProcessID;
		data.parentPid = entry.th32ParentProcessID;
		data.szExe     = entry.szExeFile;
	}

	CloseHandle(snapshot);
	return result;
}

_NODISCARD ProcessEntry GetProcess(PID pid) EXCEPT
{
	auto procList = GetAllProcess();
	auto find = procList.find(pid);

	if (find == procList.end())
	{
		throw Exception("Couldn't find the process PID=%d", pid);
	}

	return find->second;
}

// Get a list of windows, includes their titles, classname and pid
_NODISCARD std::vector<WindowEntry> GetAllWindows() EXCEPT
{
	static auto enumProc = [](HWND hwnd, LPARAM lParam) -> BOOL
	{
		auto lpResults = reinterpret_cast<std::vector<WindowEntry>*>(lParam);

		WindowEntry data{};
		wchar_t title[512], classname[512];

		GetWindowTextW(hwnd, title, 512);
		GetClassNameW(hwnd, classname, 512);
		GetWindowThreadProcessId(hwnd, &data.pid);

		data.hwnd = hwnd;
		data.title = title;
		data.classname = classname;
		lpResults->push_back(data);
		return true;
	};

	std::vector<WindowEntry> results;

	if (!EnumWindows(enumProc, reinterpret_cast<LPARAM>(&results)))
	{
		static const std::string except_msg("EnumWindows failed");
		throw Exception(except_msg);
	}

	return results;
}

bool OpenFileInExplorer(const std::wstring& filePath) noexcept
{
	LPITEMIDLIST pidl = ILCreateFromPathW(filePath.c_str());
	if (!pidl) return false;

	HRESULT hr = SHOpenFolderAndSelectItems(pidl, 0u, nullptr, 0ul);
	if (hr != S_OK) return false;

	ILFree(pidl);
	return true;
}

bool ClipboardCopy(const std::wstring& text) noexcept
{
	if (!OpenClipboard(0)) return false;
	if (!EmptyClipboard()) return false;

	HANDLE hClipboardData = GlobalAlloc(GMEM_DDESHARE, sizeof(wchar_t) * (text.size() + 1));
	if (!hClipboardData) return false;

	bool result = false;
	void* pchData = GlobalLock(hClipboardData);

	if (pchData)
	{
		memcpy(pchData, text.c_str(), text.size() * sizeof(wchar_t));

		if (GlobalUnlock(hClipboardData))
		{
			result = SetClipboardData(CF_UNICODETEXT, hClipboardData);
		}
	}
	GlobalFree(pchData);
	CloseClipboard();
	return {};
}

bool ClipboardCopy(const std::string& text) noexcept
{
	return ClipboardCopy(String(text));
}

bool ClipboardCopy(unsigned long long number, bool isHex) noexcept
{
	wchar_t text[64];
	swprintf_s(text, isHex ? L"%llx" : L"%lld", number);

	return ClipboardCopy(text);
}
}