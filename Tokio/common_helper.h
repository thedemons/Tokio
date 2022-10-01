#pragma once
#ifndef TOKIO_COMMON_HELPER_H
#define TOKIO_COMMON_HELPER_H

#define BHMAX_PATH 4096


namespace common
{

struct WindowData
{
	HWND hwnd;
	DWORD pid;
	std::wstring title;
	std::wstring classname; // class name
};

uint64_t BhGetUnixTime();

std::string  BhString          (std::wstring str);
std::wstring BhString          (std::string str);

std::wstring BhStringLower     (std::wstring str);
std::string  BhStringLower     (std::string str);
std::wstring BhStringUpper     (std::wstring str);
std::string  BhStringUpper     (std::string str);

// Path processing
std::string  BhPathParent      (std::string path        , int level = 1);
std::string  BhPathAppend      (std::string path        , std::string trail);
std::string  BhPathAppend      (std::string path        , std::vector<std::string> trails);
std::wstring BhPathParent      (std::wstring path       , int level = 1);
std::wstring BhPathAppend      (std::wstring path       , std::wstring trail);
std::wstring BhPathAppend      (std::wstring path       , std::vector<std::wstring> trails);

std::string  BhGetFileExtension(const std::string& fileName);
std::wstring BhGetFileExtension(const std::wstring& fileName);

std::wstring BhPathGetTrail    (const std::wstring& path, int level = 1);
std::string  BhPathGetTrail    (const std::string& path , int level = 1);

// Get a list of all running process
// TODO: Use another type, not PROCESSENTRY32W
_NODISCARD auto BhGetAllProcess()                                 ->SafeResult(std::map<DWORD, PROCESSENTRY32W>);
_NODISCARD auto BhGetProcessData(DWORD pid)                       ->SafeResult(PROCESSENTRY32W);

// Get a list of windows, includes their titles, classname and pid
_NODISCARD auto BhGetAllWindows()                                 ->SafeResult(std::vector<WindowData>);
_NODISCARD auto BhOpenFileInExplorer(const std::wstring& filePath)->SafeResult(void);

// clipboard
_NODISCARD auto BhClipboardCopy(const std::wstring& text)         ->SafeResult(void);
_NODISCARD auto BhClipboardCopy(const std::string& text)          ->SafeResult(void);
_NODISCARD auto BhClipboardCopy(unsigned long long number, bool isHex = true)        ->SafeResult(void);
}
#endif // !TOKIO_COMMON_HELPER_H