#pragma once
#ifndef TOKIO_ENGINE_BASESYMBOL_H
#define TOKIO_ENGINE_BASESYMBOL_H

#include "Engine/EngineDef.h"
#include "common_result.hpp"

#include <memory>
#include <vector>

namespace Engine
{

class BaseSymbol
{
protected:
	std::shared_ptr<ProcessData> m_target;

	// a copy of the target modules data in ascending address order
	std::vector<ProcessModule> m_sortedModules;

	// sort symbols, acsending order
	// this also calculate the symbol size
	void SortSymbol(ProcessModule& procMod);

	// Make a copy of the modules (m_sortedModules)
	// and sort them in acsending order
	void UpdateModules(const std::vector<ProcessModule>& modules);

public:
	BaseSymbol(const std::shared_ptr<ProcessData>& target);

	// update the module and symbol list of the
	// target process, also return a reference back to the caller
	// NOTICE: Any inherited class of BaseSymbol must call UpdateModules()
	//		   after retrieving the symbols to finalize the internal vector m_sortedModules
	virtual _NODISCARD auto Update() -> SafeResult(std::vector<ProcessModule>&) = 0;

	// format and address into module.function+0xxxx
	virtual _NODISCARD auto AddressToSymbol(POINTER address) -> SafeResult(std::string) = 0;

	// return an adddress from a symbol such as module.function+0xxxx
	virtual _NODISCARD auto SymbolToAddress(const std::string& symbol) -> SafeResult(POINTER) = 0;

	// intialize a symbol walk for sorted address
	virtual _NODISCARD _CONSTEXPR20 auto AddressSymbolWalkInit() -> SymbolWalkContext
	{
		return SymbolWalkContext(m_sortedModules);
	}

	// find next address, remember that the addresses passed into this function must be sorted
	virtual _NODISCARD auto AddressSymbolWalkNext(SymbolWalkContext& context, POINTER address)->ResultGetSymbol;

	// helper function for a single AddressSymbolWalkInit && AddressSymbolWalkNext
	virtual _NODISCARD _CONSTEXPR20 auto AddressToModuleSymbol(POINTER address) -> ResultGetSymbol
	{
		SymbolWalkContext context(m_sortedModules);
		return AddressSymbolWalkNext(context, address);
	}
};

}
#endif // !TOKIO_ENGINE_BASESYMBOL_H