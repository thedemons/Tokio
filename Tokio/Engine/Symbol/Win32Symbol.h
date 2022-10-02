#pragma once
#ifndef TOKIO_ENGINE_SYMBOL_WIN32SYMBOL_H
#define TOKIO_ENGINE_SYMBOL_WIN32SYMBOL_H

#include "Engine/EngineDef.hpp"
#include "Engine/Symbol/BaseSymbol.hpp"
#include <vector>

namespace Engine
{
class Win32Symbol : public BaseSymbol
{
	using BaseSymbol::BaseSymbol;
private:
	std::wstring m_bufferModulePath;
	std::vector<HMODULE> m_bufferModule{ 256 };

	// try using kernel32 api
	_NODISCARD auto K32RetrieveModuleList() -> SafeResult(void);

	// try using tlhelp32 api
	_NODISCARD auto Tlhelp32RetrieveModuleList() -> SafeResult(void);

public:

	_NODISCARD auto Update() -> SafeResult(std::vector<ProcessModule>&) override;

	// format and address into module.function+0xxxx
	_NODISCARD auto AddressToSymbol(POINTER address)->SafeResult(std::string) override;

	// return an adddress from a symbol such as module.function+0xxxx
	_NODISCARD auto SymbolToAddress(const std::string& symbol)->SafeResult(POINTER) override;
};

}
#endif // !TOKIO_ENGINE_SYMBOL_WIN32SYMBOL_H