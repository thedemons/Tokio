#include "stdafx.h"
#include "common_helper.h"


namespace common
{

uint64_t BhGetUnixTime()
{
	std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch());
	return seconds.count();
}


// String processing

std::wstring BhStringLower(std::wstring str)
{
	std::transform(str.begin(), str.end(), str.begin(), std::towlower);
	return str;
}

std::string BhStringLower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
	return str;
}

std::wstring BhStringUpper(std::wstring str)
{
	std::transform(str.begin(), str.end(), str.begin(), std::towupper);
	return str;
}

std::string BhStringUpper(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
	return str;
}

std::string BhString(std::wstring str)
{
	//return std::string(str.begin(), str.end());

	int size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	char* buffer = new char[size + 1];
	buffer[size] = 0;

	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), buffer, size, NULL, NULL);

	std::string result(buffer);
	delete[] buffer;

	return result;
}

std::wstring BhString(std::string str)
{
	int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0);
	wchar_t* buffer = new wchar_t[size + 1];
	buffer[size] = 0;

	MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), buffer, size);

	std::wstring result(buffer);
	delete[] buffer;

	return result;
}

// Path processing

std::string BhPathParent(std::string path, int level)
{
	// remove trailing slash
	if (path.back() == '\\') path = path.substr(0, path.size() - 1);

	for (int i = 0; i < level; i++)
	{
		path = path.substr(0, path.find_last_of('\\'));
	}
	return path;
}

std::string BhPathAppend(std::string path, std::string trail)
{
	if (path.back() != '\\') path += '\\';
	return path + trail;
}

std::string BhPathAppend(std::string path, std::vector<std::string> trails)
{
	for (auto& trail : trails)
	{
		if (path.back() != '\\') path += '\\';

		path += (trail.front() != '\\') ? trail : trail.substr(trail.find_first_not_of(L'\\'));
	}
	return path;
}

std::wstring BhPathParent(std::wstring path, int level)
{
	// remove trailing slash
	if (path.back() == L'\\') path = path.substr(0, path.size() - 1);

	for (int i = 0; i < level; i++)
	{
		path = path.substr(0, path.find_last_of(L'\\'));
	}
	return path;
}

std::wstring BhPathAppend(std::wstring path, std::wstring trail)
{
	if (path.back() != L'\\') path += L'\\';
	return path + trail;
}

std::wstring BhPathAppend(std::wstring path, std::vector<std::wstring> trails)
{
	for (auto& trail : trails)
	{
		if (path.back() != L'\\') path += L'\\';

		path += (trail.front() != L'\\') ? trail : trail.substr(trail.find_first_not_of(L'\\'));
	}
	return path;
}

std::wstring BhGetFileExtension(const std::wstring& fileName)
{
	for (size_t i = fileName.size() - 1; i > 0; i--)
	{
		if (fileName[i] == L'\\') break;
		else if (fileName[i] == L'.')
		{
			return fileName.substr(i + 1);
		}
	}
	return L"";
}

std::string BhGetFileExtension(const std::string& fileName)
{
	for (size_t i = fileName.size() - 1; i > 0; i--)
	{
		if (fileName[i] == '\\') break;
		else if (fileName[i] == '.')
		{
			return fileName.substr(i + 1);
		}
	}
	return "";
}

std::string BhPathGetTrail(const std::string& path, int level)
{
	std::string tempPath = BhPathParent(path, level);
	return path.substr(tempPath.size() + 1);
}

std::wstring BhPathGetTrail(const std::wstring& path, int level)
{
	std::wstring tempPath = BhPathParent(path, level);
	return path.substr(tempPath.size() + 1);
}

// Get a list of all running process
// TODO: Use another type, not PROCESSENTRY32W
_NODISCARD auto BhGetAllProcess() -> SafeResult(std::map<DWORD, PROCESSENTRY32W>)
{
	std::map<DWORD, PROCESSENTRY32W> result;

	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	WINAPI_FAILIFN_NM(snapshot);
	WINAPI_FAILIFN_NM(Process32FirstW(snapshot, &entry));

	while (Process32NextW(snapshot, &entry) == TRUE)
	{
		result[entry.th32ProcessID] = entry;
	}

	CloseHandle(snapshot);

	WINAPI_FAILIFN_NM(result.size());
	return result;
}

_NODISCARD auto common::BhGetProcessData(DWORD pid)->SafeResult(PROCESSENTRY32W)
{
	auto procList = BhGetAllProcess();
	RESULT_FAILIFN_PASS(procList.has_value(), procList.error());

	auto find = procList.value().find(pid);
	RESULT_FAILIFN_NM(find != procList.value().end());

	return find->second;
}

// Get a list of windows, includes their titles, classname and pid
_NODISCARD auto BhGetAllWindows() -> SafeResult(std::vector<WindowData>)
{
	static auto enumProc = [](HWND hwnd, LPARAM lParam) -> BOOL
	{
		auto lpResults = reinterpret_cast<std::vector<WindowData>*>(lParam);

		WindowData data{};
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

	std::vector<WindowData> results;
	EnumWindows(enumProc, reinterpret_cast<LPARAM>(&results));

	WINAPI_FAILIFN_NM(results.size() > 0);

	return results;
}

_NODISCARD auto BhOpenFileInExplorer(const std::wstring& filePath) -> SafeResult(void)
{
	LPITEMIDLIST pidl = ILCreateFromPathW(filePath.c_str());
	WINAPI_FAILIFN_NM(pidl);

	HRESULT hr = SHOpenFolderAndSelectItems(pidl, 0u, nullptr, 0ul);
	HRESULT_FAILIFN_NM(hr);

	ILFree(pidl);
	return {};
}

_NODISCARD auto BhClipboardCopy(const std::wstring& text)->SafeResult(void)
{
	WINAPI_FAILIFN_NM(OpenClipboard(0));
	WINAPI_FAILIFN_NM(EmptyClipboard());

	HANDLE hClipboardData = GlobalAlloc(GMEM_DDESHARE, sizeof(wchar_t) * (text.size() + 1));
	WINAPI_FAILIFN_NM(hClipboardData);

	void* pchData = GlobalLock(hClipboardData);
	WINAPI_FAILIFN_NM(pchData);

	memcpy(pchData, text.c_str(), text.size() * sizeof(wchar_t));

	GlobalUnlock(hClipboardData);
	WINAPI_FAILIFN_NM(pchData);

	SetClipboardData(CF_UNICODETEXT, hClipboardData);
	WINAPI_FAILIFN_NM(pchData);

	CloseClipboard();
	WINAPI_FAILIFN_NM(pchData);

	return {};
}

_NODISCARD auto BhClipboardCopy(const std::string& text)->SafeResult(void)
{
	return BhClipboardCopy(BhString(text));
}
auto BhClipboardCopy(unsigned long long number, bool isHex)->SafeResult(void)
{
	wchar_t text[64];
	swprintf_s(text, isHex ? L"%llx" : L"%lld", number);

	return BhClipboardCopy(text);
}
}