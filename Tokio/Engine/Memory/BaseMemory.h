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

#pragma once
#ifndef TOKIO_ENGINE_BASEMEMORY_H
#define TOKIO_ENGINE_BASEMEMORY_H

#include "Engine/EngineDef.h"
#include "Common/Exception.h"

#include <memory>

namespace Engine
{
class BaseMemory
{
protected:
	std::shared_ptr<ProcessData> m_target;

public:
	// attach to a process
	_NODISCARD virtual std::shared_ptr<ProcessData>
	Attach(PID pid) EXCEPT = 0;
	
	// detach the already-attached process
	virtual void Detach() noexcept = 0;

	// read memory into a buffer
	_NODISCARD virtual size_t Read(
		POINTER src,
		void* dest,
		size_t size
	) const noexcept = 0;

	// write to the target process memory
	_NODISCARD virtual size_t Write(
		POINTER dest,
		const void* src,
		size_t size
	) const noexcept = 0;

	// Read memory of the region that may cross a no-read-access page
	virtual void ReadMemSafe(
		POINTER address,
		byte_t* buffer,
		size_t size,
		std::vector<MemoryReadRegion>& regions
	) const noexcept;

	// query virtual memory information from the target process
	_NODISCARD virtual VirtualMemoryInfo VirtualQuery(
		POINTER address
	) const EXCEPT = 0;

	// template for quick reading
	template <typename Type>
	_NODISCARD Type Read(POINTER address) const EXCEPT 
	{
		Type value;
		size_t readSize = Read(address, &value, sizeof(Type));

		if (readSize != sizeof(Type))
		{
			throw Tokio::Exception("Memory engine didn't"
				" read enough size for template typename Type");
		}

		return value;
	}

	// template for quick writing
	template <typename Type>
	_NODISCARD size_t Write(POINTER address, const Type& value) const noexcept
	{
		return Write(address, &value, sizeof(Type));
	}

	// get the target process pid
	_NODISCARD virtual _CONSTEXPR20 PID GetPID() const noexcept
	{
		return m_target->pid;
	}

	// get a handle to the target process
	_NODISCARD virtual _CONSTEXPR20 ProcessHandle GetHandle() const noexcept
	{
		return m_target->handle;
	}
};
}

#endif // !TOKIO_ENGINE_BASEMEMORY_H