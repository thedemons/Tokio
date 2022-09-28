#pragma once

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

std::wstring BhGetFileExtension(std::wstring fileName)
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

std::string BhGetFileExtension(std::string fileName)
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

std::string BhPathGetTrail(std::string path, int level)
{
	std::string tempPath = BhPathParent(path, level);
	return path.substr(tempPath.size() + 1);
}

std::wstring BhPathGetTrail(std::wstring path, int level)
{
	std::wstring tempPath = BhPathParent(path, level);
	if (tempPath.size() >= path.size()) return path;
	return path.substr(tempPath.size() + 1);
}


// Windows directories

std::wstring BhGetTempDir()
{
	wchar_t filePath[BHMAX_PATH];
	GetTempPathW(BHMAX_PATH, filePath);
	return filePath;
}

std::wstring BhGetWindowsDir()
{
	wchar_t filePath[BHMAX_PATH];
	GetWindowsDirectoryW(filePath, BHMAX_PATH);
	return filePath;
}

std::wstring BhGetSystem32Dir()
{
	wchar_t filePath[BHMAX_PATH];
	GetSystemDirectoryW(filePath, BHMAX_PATH);
	return filePath;
}

std::wstring BhGetTempFile(LPCWSTR file)
{
	std::wstring filePath = BhGetTempDir();
	if (filePath.back() != L'\\') filePath += L'\\';

	return filePath + file;
}

std::wstring BhGetWindowsFile(LPCWSTR file)
{
	std::wstring filePath = BhGetWindowsDir();
	if (filePath.back() != L'\\') filePath += L'\\';

	return filePath + file;
}

std::wstring BhGetSystem32File(LPCWSTR file)
{
	std::wstring filePath = BhGetSystem32Dir();
	if (filePath.back() != L'\\') filePath += L'\\';

	return filePath + file;
}

// Processes

auto BhGetAllProcess() -> SafeResult(std::map<DWORD, PROCESSENTRY32W>)
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

bool BhProcessExists(DWORD pid)
{
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32FirstW(snapshot, &entry) == TRUE)
	{
		while (Process32NextW(snapshot, &entry) == TRUE)
		{
			if (entry.th32ProcessID == pid)
			{
				CloseHandle(snapshot);
				return true;
			}
		}
	}

	CloseHandle(snapshot);
	return false;
}

DWORD BhProcessExists(const wchar_t* processName)
{
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32FirstW(snapshot, &entry) == TRUE)
	{
		while (Process32NextW(snapshot, &entry) == TRUE)
		{
			if (_wcsicmp(entry.szExeFile, processName) == 0)
			{
				CloseHandle(snapshot);
				return entry.th32ProcessID;
			}
		}
	}

	CloseHandle(snapshot);
	return 0;
}


DWORD BhProcessExistsEasy(std::wstring processName)
{
	std::wstring wstrLowerProcName = BhStringLower(processName);

	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32FirstW(snapshot, &entry) == TRUE)
	{
		while (Process32NextW(snapshot, &entry) == TRUE)
		{
			std::wstring wstrLowerTargetName = BhStringLower(entry.szExeFile);

			if (wstrLowerTargetName.find(wstrLowerProcName) != std::wstring::npos)
			{
				CloseHandle(snapshot);
				return entry.th32ProcessID;
			}
		}
	}

	CloseHandle(snapshot);
	return 0;
}

PROCESSENTRY32W BhFindProcessEntry(DWORD pid)
{
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);
	entry.th32ProcessID = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32FirstW(snapshot, &entry) == TRUE)
	{
		while (Process32NextW(snapshot, &entry) == TRUE)
		{
			if (entry.th32ProcessID == pid)
			{
				CloseHandle(snapshot);
				return entry;
			}
		}
	}

	PROCESSENTRY32W empty;
	empty.dwSize = sizeof(PROCESSENTRY32W);
	empty.th32ProcessID = 0;

	return empty;
}

PROCESSENTRY32W GetRootProcess(std::map<DWORD, PROCESSENTRY32W> processes, PROCESSENTRY32W process)
{
	PROCESSENTRY32W root = process;
	while (root.th32ParentProcessID != 0)
	{
		auto find = processes.find(root.th32ParentProcessID);
		if (find == processes.end()) return root;
		root = find->second;
	}
	return root;
}

// windows
auto BhGetAllWindows() -> SafeResult(std::vector<WindowData>)
{
	std::vector<WindowData> results;

	static auto enumProc = [](HWND hwnd, LPARAM lParam)  -> BOOL __stdcall
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

	EnumWindows(enumProc, reinterpret_cast<LPARAM>(&results));
	WINAPI_FAILIFN_NM(results.size() > 0);

	return results;
}


// files
std::wstring BhFindBackupFilePath(std::wstring mainPath)
{
	std::wstring extension;

	for (size_t i = mainPath.size(); i > 0; i--)
	{
		if (mainPath[i] == L'\\')
		{
			extension = L"";
			break;
		}
		else if (mainPath[i] == L'.')
		{
			extension = mainPath.substr(i);
			mainPath = mainPath.substr(0, i);
		}
	}

	for (int i = 0;; i++)
	{
		wchar_t subpath[10];
		ZeroMemory(subpath, sizeof(subpath));

		_itow_s(i, subpath, 10);
		std::wstring path = mainPath + subpath + extension;
		if (!std::filesystem::exists(path.c_str()))
		{
			return path;
		}
	}
}


std::string BhFindBackupFilePath(std::string mainPath)
{
	std::string extension;

	for (size_t i = mainPath.size(); i > 0; i--)
	{
		if (mainPath[i] == '\\')
		{
			extension = "";
			break;
		}
		else if (mainPath[i] == L'.')
		{
			extension = mainPath.substr(i);
			mainPath = mainPath.substr(0, i);
		}
	}

	for (int i = 0;; i++)
	{
		char subpath[10];
		ZeroMemory(subpath, sizeof(subpath));

		_itoa_s(i, subpath, 10);
		std::string path = mainPath + subpath + extension;
		if (!std::filesystem::exists(path.c_str()))
		{
			return path;
		}
	}
}


// file processing
bool BhFileReadToByte(std::wstring filePath, std::vector<BYTE>* output, size_t max_len)
{
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open()) return false;

	file.seekg(0, file.end);
	size_t length = file.tellg();
	file.seekg(0, file.beg);

	length = (length > max_len) ? max_len : length;

	output->clear();
	output->resize(length);
	file.read((char*)output->data(), length);

	file.close();
	return true;
}

bool BhFileExists(std::wstring filePath)
{
	return std::filesystem::exists(filePath.c_str());
}

}