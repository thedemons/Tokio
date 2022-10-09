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
#include "BaseMemory.h"

namespace Engine
{

// Read memory of the region that may cross a no-read-access page
void BaseMemory::ReadMemSafe(
	POINTER address,
	byte_t* buffer,
	size_t size,
	std::vector<MemoryReadRegion>& regions
) const noexcept
{
	VirtualMemoryInfo memInfo;

	try
	{
		memInfo =  VirtualQuery(address);
	}
	catch (Tokio::Exception& e)
	{
		e.Log("ReadMemSafe couldn't query virtual memory information\n");
		return;
	}


	POINTER endAddress = memInfo.base + memInfo.size;
	size_t offset = endAddress - address;

	// if the region is readable
	if (memInfo.protect & VirtualProtection::Read)
	{
		auto& region = regions.emplace_back();

		// if the whole size is readable
		if (endAddress >= address + size)
		{
			region.start = address;
			region.size = Read(address, buffer, size);

			if (region.size != size)
			{
				Tokio::Log("ReadMemSafe only read %d/%d bytes\n", region.size, size);
			}
		}
		else
		{
			size_t readSize = endAddress - address;
			region.start = address;
			region.size = Read(address, buffer, readSize);

			if (region.size != readSize)
			{
				Tokio::Log("ReadMemSafe only read %d/%d bytes\n", region.size, readSize);
			}

			return ReadMemSafe(endAddress, buffer + region.size, size - offset, regions);
		}
	}
	// there are still readable regions left
	else if (endAddress <= address + size)
	{
		return ReadMemSafe(endAddress, buffer + offset, size - offset, regions);
	}

}

}