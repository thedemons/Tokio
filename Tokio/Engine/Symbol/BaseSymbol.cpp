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
#include "BaseSymbol.h"

namespace Engine
{

BaseSymbol::BaseSymbol(const std::shared_ptr<ProcessData>& target) EXCEPT : m_target(target)
{
}

// sort symbols, ascending order
// this also calculate the symbol size
void BaseSymbol::SortSymbol(ProcessModule& procMod)
{
	if (procMod.symbols.size() == 0) return;

	static const auto lmSortSymbolAsc = [](const ModuleSymbol& a, const ModuleSymbol& b) -> bool
	{
		return a.offset < b.offset;
	};

	std::sort(procMod.symbols.begin(), procMod.symbols.end(), lmSortSymbolAsc);

	// calculate the size of the symbols by subtracting the offset of the next symbol by this symbol offset
	for (size_t i = 0; i < procMod.symbols.size() - 1; i++)
	{
		procMod.symbols[i].size = procMod.symbols[i + 1].offset - procMod.symbols[i].offset - 1;
	}

	// default the last symbol size to 0x10 // FIXME
	procMod.symbols.back().size = 0x10;
}

// Make a copy of the modules (m_sortedModules)
// and sort them in ascending order
void BaseSymbol::UpdateModules(std::vector<ProcessModule>& modules)
{
	if (modules.size() == 0) return;

	m_sortedModules.clear();
	m_sortedModules.reserve(modules.size());

	for (ProcessModule& procMod : modules)
	{
		m_sortedModules.push_back(procMod);
	}

	static const auto lmSortModuleAsc = [](const ProcessModule& a, const ProcessModule& b) -> bool
	{
		return a.base < b.base;
	};

	std::sort(m_sortedModules.begin(), m_sortedModules.end(), lmSortModuleAsc);

	for (auto& procMod : m_sortedModules)
	{
		SortSymbol(procMod);
	}
}

// find next address, remember that the addresses passed into this function must be sorted
_NODISCARD ResultGetSymbol BaseSymbol::AddressSymbolWalkNext(SymbolWalkContext& context, POINTER address) const noexcept
{
	// FIXME: THIS COULD BE OPTIMIZED FURTHER BY SAVING THE INDEX OF THE NEAREST MODULE

	context.BeginWalk();

	// walking the modules
	do
	{
		auto pModule = context.Module();
		auto pSymbol = context.Symbol();

		// if the address is inside the module
		if (pModule->base <= address && address <= pModule->base + pModule->size)
		{
			// if the module_index is == UPTR_UNDEFINED
			// means we haven't walked any symbols for this module yet
			if (pSymbol == nullptr)
			{
				// if the address don't belong to any symbols, then it must belongs to the module
				// the reason we put || !context.NextSymbol() in here is because it is [[nodiscard]]
				// if the first condition is true then the second condition is always true and vice versa
				if (!context.IsAddressOfSymbols(address) || !context.NextSymbol()) return ResultGetSymbol(pModule);
			}

			// relative address (offset)
			POINTER addressRVa = address - pModule->base;

			// walking the symbols
			do
			{
				pSymbol = context.Symbol();

				// the address belongs to this symbol
				if (pSymbol->offset <= addressRVa && addressRVa <= pSymbol->offset + pSymbol->size)
					return ResultGetSymbol(pModule, pSymbol);

			} while (context.NextSymbol());

			// not found in any symbols but still belongs to the module
			return ResultGetSymbol(pModule);
		}

	} while (context.NextModule());

	// found none, revert the index back
	context.EndWalk();

	return ResultGetSymbol();
}

}
