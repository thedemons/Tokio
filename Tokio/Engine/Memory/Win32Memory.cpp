#include "stdafx.h"
#include "Win32Memory.hpp"

namespace Engine
{

Win32Memory::~Win32Memory()
{
	Detach();
}

_NODISCARD auto Win32Memory::Attach(DWORD pid)->SafeResult(std::shared_ptr<ProcessData>)
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

void Win32Memory::Detach()
{
	if (m_target == nullptr) return;

	CloseHandle(m_target->handle);

	// release the shared pointer
	m_target.reset();
}

_NODISCARD auto Win32Memory::Read(POINTER src, void* dest, size_t size)->SafeResult(void)
{
	SIZE_T bytesRead = 0;
	BOOL readResult = ReadProcessMemory(m_target->handle, reinterpret_cast<LPCVOID>(src), dest, size, &bytesRead);

	WINAPI_FAILIFN(readResult, ReadProcessMemoryFailed);
	WINAPI_FAILIFN(bytesRead == size, ReadProcessMemoryNotEnoughByte);

	return {};
}

_NODISCARD auto Win32Memory::Write(POINTER dest, const void* src, size_t size)->SafeResult(void)
{
	SIZE_T bytesWritten = 0;
	BOOL writeResult = WriteProcessMemory(m_target->handle, reinterpret_cast<LPVOID>(dest), src, size, &bytesWritten);

	WINAPI_FAILIFN(writeResult, ReadProcessMemoryFailed);
	WINAPI_FAILIFN(bytesWritten == size, WriteProcessMemoryNotEnoughByte);

	return {};
}

_NODISCARD auto Win32Memory::VirtualQuery(POINTER address)->SafeResult(VirtualMemoryInfo)
{
	MEMORY_BASIC_INFORMATION info;
	size_t result = ::VirtualQueryEx(m_target->handle, reinterpret_cast<LPCVOID>(address), &info, sizeof(info));
	WINAPI_FAILIFN(result != 0, VirtualQueryFailed);

	assert(result == sizeof(info));

	VirtualMemoryInfo  memInfo;
	memInfo.base = reinterpret_cast<POINTER>(info.BaseAddress);
	memInfo.size = info.RegionSize;
	//memInfo.protect    = info.Protect;
	switch (info.Protect)
	{
		case PAGE_NOACCESS:
			memInfo.protect = VirtualProtection::NoAccess;
			break;

		case PAGE_GUARD:
			memInfo.protect = VirtualProtection::Guard;
			break;

		case PAGE_EXECUTE:
			memInfo.protect = VirtualProtection::Execute;
			break;

		case PAGE_EXECUTE_READ:
			memInfo.protect = VirtualProtection::Execute | VirtualProtection::Read;
			break;

		case PAGE_EXECUTE_READWRITE:
			memInfo.protect = VirtualProtection::Execute | VirtualProtection::Read | VirtualProtection::Write;
			break;

		case PAGE_EXECUTE_WRITECOPY:
			memInfo.protect = VirtualProtection::Execute | VirtualProtection::WriteCopy;
			break;

		case PAGE_READONLY:
			memInfo.protect = VirtualProtection::Read;
			break;

		case PAGE_READWRITE:
			memInfo.protect = VirtualProtection::Read | VirtualProtection::Write;
			break;

		case PAGE_WRITECOPY:
			memInfo.protect = VirtualProtection::WriteCopy;
			break;
	}

	return memInfo;
}

};
