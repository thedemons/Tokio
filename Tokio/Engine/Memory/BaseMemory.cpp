#include "stdafx.h"
#include "BaseMemory.h"

namespace Engine
{

// Read memory of the region that may cross a no-read-access page
_NODISCARD auto BaseMemory::ReadMemSafe(POINTER address, BYTE* buffer, size_t size, std::vector<MemoryRegion>& regions)->SafeResult(void)
{
	auto resultQuery = VirtualQuery(address);

	if (resultQuery.has_error())
	{
		assert(false && "ViewDisassembler could not query the start address information");
		RESULT_FORWARD(resultQuery);
	}

	VirtualMemoryInfo& memInfo = resultQuery.value();
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
			region.size = size;

			if (auto resultRead = Read(address, buffer, size); resultRead.has_error())
			{
				assert(false && "ReadMemSafe failed");
				RESULT_FORWARD(resultRead);
			}

			return {};
		}
		else
		{
			region.start = address;
			region.size = endAddress - address;

			if (auto resultRead = Read(address, buffer, region.size); resultRead.has_error())
			{
				assert(false && "ReadMemSafe failed");
				RESULT_FORWARD(resultRead);
			}

			return ReadMemSafe(endAddress, buffer + region.size, size - offset, regions);
		}
	}
	// there is still readable region left
	else if (endAddress <= address + size)
	{
		return ReadMemSafe(endAddress, buffer + offset, size - offset, regions);
	}

	return {};
}

}