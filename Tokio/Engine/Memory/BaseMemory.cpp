#include "stdafx.h"
#include "BaseMemory.h"

namespace Engine
{

// Read memory of the region that may cross a no-read-access page
void BaseMemory::ReadMemSafe(
	POINTER address,
	BYTE* buffer,
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