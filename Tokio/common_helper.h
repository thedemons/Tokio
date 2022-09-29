#pragma once
#ifndef TOKIO_COMMON_HELPER_HPP
#define TOKIO_COMMON_HELPER_HPP

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
std::wstring BhStringLower(std::wstring str);
std::string BhStringLower(std::string str);
std::wstring BhStringUpper(std::wstring str);
std::string BhStringUpper(std::string str);
std::string BhString(std::wstring str);
std::wstring BhString(std::string str);

// Path processing
std::string BhPathParent(std::string path, int level = 1);
std::string BhPathAppend(std::string path, std::string trail);
std::string BhPathAppend(std::string path, std::vector<std::string> trails);
std::string BhGetFileExtension(std::string fileName);
std::wstring BhPathParent(std::wstring path, int level = 1);
std::wstring BhPathAppend(std::wstring path, std::wstring trail);
std::wstring BhPathAppend(std::wstring path, std::vector<std::wstring> trails);
std::wstring BhGetFileExtension(std::wstring fileName);
std::wstring BhPathGetTrail(std::wstring path, int level = 1);
std::string BhPathGetTrail(std::string path, int level = 1);

// Get a list of all running process
// TODO: Use another type, not PROCESSENTRY32W
auto BhGetAllProcess()->SafeResult(std::map<DWORD, PROCESSENTRY32W>);
auto BhGetProcessData(DWORD pid)->SafeResult(PROCESSENTRY32W);

// Get a list of windows, includes their titles, classname and pid
auto BhGetAllWindows()->SafeResult(std::vector<WindowData>);
auto BhOpenFileInExplorer(const std::wstring& filePath)->SafeResult(void);
}
#endif