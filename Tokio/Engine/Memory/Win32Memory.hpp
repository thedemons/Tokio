#pragma once

class Win32Memory : public BaseMemory
{
private:
	std::vector<HMODULE> m_bufferModule = std::vector<HMODULE>(126);
	std::wstring m_bufferModulePath;

public:

	auto Attach(DWORD pid) -> SafeResult(void) override
	{
		m_handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		WINAPI_FAILIFN(m_handle, AttachProcessFailed);

		m_pid = pid;

		return {};
	}

	auto Read(POINTER src, void* dest, size_t size) -> SafeResult(void) override
	{
		SIZE_T bytesRead = 0;
		BOOL readResult = ReadProcessMemory(m_handle, reinterpret_cast<LPCVOID>(src), dest, size, &bytesRead);

		WINAPI_FAILIFN(readResult, ReadProcessMemoryFailed);
		WINAPI_FAILIFN(bytesRead == size, ReadProcessMemoryNotEnoughByte);

		return {};
	}

	auto Write(POINTER dest, const void* src, size_t size) -> SafeResult(void) override
	{
		SIZE_T bytesWritten = 0;
		BOOL writeResult = WriteProcessMemory(m_handle, reinterpret_cast<LPVOID>(dest), src, size, &bytesWritten);

		WINAPI_FAILIFN(writeResult, ReadProcessMemoryFailed);
		WINAPI_FAILIFN(bytesWritten == size, WriteProcessMemoryNotEnoughByte);

		return {};
	}

	virtual auto GetModuleList() -> SafeResult(std::vector<ModuleData>)
	{

		DWORD cbSizeNeeded = 0;
		DWORD cbCurrentSize = static_cast<DWORD>(m_bufferModule.size() * sizeof(HMODULE));

		BOOL result =
			K32EnumProcessModulesEx(
				m_handle, 
				m_bufferModule.data(),
				cbCurrentSize,
				&cbSizeNeeded,
				LIST_MODULES_ALL
			);

		WINAPI_FAILIFN(result, EnumProcessModulesFailed);

		// the hMods didn't have enough space, create more
		if (cbSizeNeeded > cbCurrentSize)
		{
			m_bufferModule.resize(static_cast<size_t>(cbSizeNeeded) / sizeof(HMODULE));
			result =
				K32EnumProcessModulesEx(
					m_handle,
					m_bufferModule.data(),
					cbCurrentSize,
					&cbSizeNeeded,
					LIST_MODULES_ALL
				);

			WINAPI_FAILIFN(result, EnumProcessModulesFailed);
		}

		size_t moduleCount = static_cast<size_t>(cbSizeNeeded) / sizeof(HMODULE);

		std::vector<ModuleData> moduleList;
		moduleList.reserve(moduleCount);

		if (m_bufferModulePath.size() == 0) m_bufferModulePath.resize(2048);

		DWORD cbBufferModule = static_cast<DWORD>(m_bufferModulePath.size() * sizeof(wchar_t));

		for (size_t i = 0; i < moduleCount; i++)
		{
			if (!m_bufferModule[i]) continue;

			ModuleData modData;
			modData.baseAddress = reinterpret_cast<POINTER>(m_bufferModule[i]);

			if (
				K32GetModuleFileNameExW(
					m_handle,
					m_bufferModule[i],
					m_bufferModulePath.data(),
					cbBufferModule
				)
			)
			{
				size_t length = wcsnlen_s(m_bufferModulePath.c_str(), m_bufferModulePath.size());
				
				modData.modulePathW = std::wstring(m_bufferModulePath.c_str(), length);
				modData.modulePathA = common::BhString(modData.modulePathW);
				modData.moduleNameW = common::BhPathGetTrail(modData.modulePathW);
				modData.moduleNameA = common::BhString(modData.moduleNameW);
			}
			else
			{
				modData.moduleNameA = "<NULL>";
				modData.moduleNameW = L"<NULL>";
			}

			moduleList.push_back(modData);
		}

		return moduleList;
	}

};