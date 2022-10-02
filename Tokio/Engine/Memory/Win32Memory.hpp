#pragma once

namespace Engine
{

class Win32Memory : public BaseMemory
{
private:
public:

	~Win32Memory()
	{
		Detach();
	}

	_NODISCARD auto Attach(DWORD pid) -> SafeResult(std::shared_ptr<ProcessData>) override
	{
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		WINAPI_FAILIFN(hProc, AttachProcessFailed);

		m_target = std::make_shared<ProcessData>();
		m_target->handle = hProc;
		m_target->pid = pid;

		BOOL getArchResult = IsWow64Process(m_target->handle, &m_target->is32bit);
		if (!getArchResult)
		{
			Detach();
			WINAPI_THROW(CannotGetTargetProcesArchitecture);
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

	_NODISCARD auto Read(POINTER src, void* dest, size_t size) -> SafeResult(void) override
	{
		SIZE_T bytesRead = 0;
		BOOL readResult = ReadProcessMemory(m_target->handle, reinterpret_cast<LPCVOID>(src), dest, size, &bytesRead);

		WINAPI_FAILIFN(readResult, ReadProcessMemoryFailed);
		WINAPI_FAILIFN(bytesRead == size, ReadProcessMemoryNotEnoughByte);

		return {};
	}

	_NODISCARD auto Write(POINTER dest, const void* src, size_t size) -> SafeResult(void) override
	{
		SIZE_T bytesWritten = 0;
		BOOL writeResult = WriteProcessMemory(m_target->handle, reinterpret_cast<LPVOID>(dest), src, size, &bytesWritten);

		WINAPI_FAILIFN(writeResult, ReadProcessMemoryFailed);
		WINAPI_FAILIFN(bytesWritten == size, WriteProcessMemoryNotEnoughByte);

		return {};
	}


};
}