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
#ifndef TOKIO_ENGINE_BASESYMBOL_H
#define TOKIO_ENGINE_BASESYMBOL_H

#include "Engine/EngineDef.h"
#include "Common/Exception.h"

#include <memory>
#include <vector>


// TODO: WE NEED TO MAKE m_sortedModules A VECTOR OF REFERENCE USING std::reference_wrapper
namespace Engine
{

class BaseSymbol
{
protected:
	std::shared_ptr<ProcessData> m_target;

	// a copy of the target modules data in ascending address order
	std::vector<std::reference_wrapper<ProcessModule>> m_sortedModules;

	// sort symbols, acsending order
	// this also calculate the symbol size
	void SortSymbol(ProcessModule& procMod);

	// Make a copy of the modules (m_sortedModules)
	// and sort them in acsending order
	void UpdateModules(std::vector<ProcessModule>& modules);

public:
	BaseSymbol(const std::shared_ptr<ProcessData>& target) EXCEPT;

	// update the module and symbol list of the
	// target process, also return a reference back to the caller
	// NOTICE: Any inherited class of BaseSymbol must call UpdateModules()
	//		   after retrieving the symbols to finalize the internal vector m_sortedModules
	virtual std::vector<ProcessModule>& Update() EXCEPT = 0;

	// format and address into module.function+0xxxx
	_NODISCARD virtual bool AddressToSymbol(POINTER address, std::string& symbol, size_t size = 512) const noexcept = 0;

	// return an adddress from a symbol such as module.function+0xxxx
	_NODISCARD virtual POINTER SymbolToAddress(const std::string& symbol) const noexcept = 0;

	// intialize a symbol walk for sorted address
	_NODISCARD virtual _CONSTEXPR20 SymbolWalkContext AddressSymbolWalkInit() const noexcept
	{
		return SymbolWalkContext(m_sortedModules);
	}

	// find next address, remember that the addresses passed into this function must be sorted
	_NODISCARD virtual ResultGetSymbol AddressSymbolWalkNext(SymbolWalkContext& context, POINTER address) const noexcept;

	// helper function for a single AddressSymbolWalkInit && AddressSymbolWalkNext
	_NODISCARD _CONSTEXPR20 virtual ResultGetSymbol AddressToModuleSymbol(POINTER address) const noexcept
	{
		SymbolWalkContext context(m_sortedModules);
		return AddressSymbolWalkNext(context, address);
	}
};

}
#endif // !TOKIO_ENGINE_BASESYMBOL_H