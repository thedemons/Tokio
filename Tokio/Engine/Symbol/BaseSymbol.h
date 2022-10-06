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
	std::vector<ProcessModule> m_sortedModules;

	// sort symbols, acsending order
	// this also calculate the symbol size
	void SortSymbol(ProcessModule& procMod);

	// Make a copy of the modules (m_sortedModules)
	// and sort them in acsending order
	void UpdateModules(const std::vector<ProcessModule>& modules);

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