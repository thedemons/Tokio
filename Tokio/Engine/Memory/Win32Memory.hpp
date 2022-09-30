#pragma once

class Win32Memory : public BaseMemory
{
private:
	std::vector<HMODULE> m_bufferModule = std::vector<HMODULE>(126);
	std::wstring m_bufferModulePath;

public:

	~Win32Memory()
	{
		Detach();
	}

	auto Attach(DWORD pid) -> SafeResult(std::shared_ptr<ProcessData>) override
	{
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		WINAPI_FAILIFN(hProc, AttachProcessFailed);

		m_target = std::make_shared<ProcessData>();
		m_target->handle = hProc;
		m_target->pid = pid;

		if (auto result = RetrieveModuleList(); result.has_error())
		{
			result.error().show();
		}
		return m_target;
	}

	void Detach() override
	{
		if (m_target == nullptr) return;

		// close the handle
		CloseHandle(m_target->handle);

		// release the shared pointer
		m_target.reset();
	}

	auto Read(POINTER src, void* dest, size_t size) -> SafeResult(void) override
	{
		SIZE_T bytesRead = 0;
		BOOL readResult = ReadProcessMemory(m_target->handle, reinterpret_cast<LPCVOID>(src), dest, size, &bytesRead);

		WINAPI_FAILIFN(readResult, ReadProcessMemoryFailed);
		WINAPI_FAILIFN(bytesRead == size, ReadProcessMemoryNotEnoughByte);

		return {};
	}

	auto Write(POINTER dest, const void* src, size_t size) -> SafeResult(void) override
	{
		SIZE_T bytesWritten = 0;
		BOOL writeResult = WriteProcessMemory(m_target->handle, reinterpret_cast<LPVOID>(dest), src, size, &bytesWritten);

		WINAPI_FAILIFN(writeResult, ReadProcessMemoryFailed);
		WINAPI_FAILIFN(bytesWritten == size, WriteProcessMemoryNotEnoughByte);

		return {};
	}

	auto RetrieveModuleList() -> SafeResult(void)
	{
		// init 256 module, if it wasn't enough we will allocate more
		if (m_bufferModule.size() == 0) m_bufferModule.resize(256);


		DWORD cbSizeNeeded = 0;
		DWORD cbCurrentSize = static_cast<DWORD>(m_bufferModule.size() * sizeof(HMODULE));

		// enum all the modules
		BOOL result =
			K32EnumProcessModulesEx(
				m_target->handle,
				m_bufferModule.data(),
				cbCurrentSize,
				&cbSizeNeeded,
				LIST_MODULES_ALL
			);

		WINAPI_FAILIFN(result, EnumProcessModulesFailed);

		// the m_bufferModule size is not enough didn't have enough space, create more
		if (cbSizeNeeded > cbCurrentSize)
		{
			m_bufferModule.resize(static_cast<size_t>(cbSizeNeeded) / sizeof(HMODULE));
			result =
				K32EnumProcessModulesEx(
					m_target->handle,
					m_bufferModule.data(),
					cbCurrentSize,
					&cbSizeNeeded,
					LIST_MODULES_ALL
				);

			WINAPI_FAILIFN(result, EnumProcessModulesFailed);
		}

		size_t moduleCount = static_cast<size_t>(cbSizeNeeded) / sizeof(HMODULE);

		// clear the modules list, just to make sure
		m_target->modules.clear();
		m_target->modules.reserve(moduleCount);

		if (m_bufferModulePath.size() == 0) m_bufferModulePath.resize(2048);

		DWORD cbBufferModule = static_cast<DWORD>(m_bufferModulePath.size() * sizeof(wchar_t));

		// iterate through each module
		for (size_t i = 0; i < moduleCount; i++)
		{
			// some module is nullptr, no idea why
			if (!m_bufferModule[i]) continue;

			ModuleData modData;
			modData.address = reinterpret_cast<POINTER>(m_bufferModule[i]);

			// get the full file path of the module
			if (
				K32GetModuleFileNameExW(
					m_target->handle,
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

			m_target->modules.push_back(modData);
		}

		return {};
	}

};