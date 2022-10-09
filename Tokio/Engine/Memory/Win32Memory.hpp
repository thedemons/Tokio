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
#ifndef TOKIO_ENGINE_MEMORY_WIN32MEMORY_H
#define TOKIO_ENGINE_MEMORY_WIN32MEMORY_H

#include "BaseMemory.h"

namespace Engine
{
class Win32Memory : public BaseMemory
{
private:
public:
	~Win32Memory() noexcept;

	_NODISCARD std::shared_ptr<ProcessData>
	Attach(PID pid) EXCEPT override;

	void Detach() noexcept override;

	_NODISCARD size_t Read(
		POINTER src,
		void* dest,
		size_t size
	) const noexcept override;

	_NODISCARD size_t Write(
		POINTER dest,
		const void* src, 
		size_t size
	) const noexcept override;

	_NODISCARD VirtualMemoryInfo VirtualQuery(
		POINTER address
	) const EXCEPT override;

};
}
#endif // !TOKIO_ENGINE_MEMORY_WIN32MEMORY_H
