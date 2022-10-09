/*
 * Project Tokio
 * Author: thedemons
 * Github: thedemons/Tokio
 * Made:   With love
 *
 * License: MIT License
 *
 * Copyright(c) 2022 thedemons
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "stdafx.h"
#include "Win32Memory.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <processthreadsapi.h>
#include <wow64apiset.h>
#include <handleapi.h>

namespace Engine
{

Win32Memory::~Win32Memory() noexcept
{
	Detach();
}

_NODISCARD std::shared_ptr<ProcessData>
Win32Memory::Attach(PID pid) EXCEPT
{
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (hProc == nullptr)
	{
		static const std::string except_openproc_failed("Win32Memory OpenProcess failed");
		throw Tokio::Exception(except_openproc_failed, Tokio::Exception::Type::WinAPI);
	}

	m_target = std::make_shared<ProcessData>();
	m_target->handle = hProc;
	m_target->pid = pid;

	BOOL result = FALSE;
	BOOL getArchResult = IsWow64Process(m_target->handle, &result);
	if (!getArchResult)
	{
		Detach();
		static const std::string except_getarch_failed("Win32Memory cannot get the target process architecture");
		throw Tokio::Exception(except_getarch_failed, Tokio::Exception::Type::WinAPI);
	}

	m_target->arch = result ? Architecture::x86 : Architecture::x86_64;
	return m_target;
}

void Win32Memory::Detach() noexcept
{
	if (m_target == nullptr) return;

	CloseHandle(m_target->handle);

	// release the shared pointer
	m_target.reset();
}

_NODISCARD size_t Win32Memory::Read(
	POINTER src,
	void* dest,
	size_t size
) const noexcept
{
	// TODO: Error logging
	SIZE_T bytesRead = 0;
	ReadProcessMemory(m_target->handle, reinterpret_cast<LPCVOID>(src), dest, size, &bytesRead);

	return bytesRead;
}

_NODISCARD size_t Win32Memory::Write(
	POINTER dest,
	const void* src,
	size_t size
) const noexcept
{
	// TODO: Error logging
	SIZE_T bytesWritten = 0;
	WriteProcessMemory(m_target->handle, reinterpret_cast<LPVOID>(dest), src, size, &bytesWritten);

	return bytesWritten;
}

_NODISCARD VirtualMemoryInfo Win32Memory::VirtualQuery(
	POINTER address
) const EXCEPT
{
	MEMORY_BASIC_INFORMATION info;
	size_t result = ::VirtualQueryEx(m_target->handle, reinterpret_cast<LPCVOID>(address), &info, sizeof(info));

	if (result != sizeof(info))
	{
		static const std::string except_virtual_query_failed("Win32Memory cannot query virtual memory information");
		throw Tokio::Exception(except_virtual_query_failed, Tokio::Exception::WinAPI);
	}

	VirtualMemoryInfo  memInfo;
	memInfo.base = reinterpret_cast<POINTER>(info.BaseAddress);
	memInfo.size = info.RegionSize;

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
