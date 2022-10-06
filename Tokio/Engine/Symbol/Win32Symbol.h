#pragma once
#ifndef TOKIO_ENGINE_SYMBOL_WIN32SYMBOL_H
#define TOKIO_ENGINE_SYMBOL_WIN32SYMBOL_H

#include "Engine/Symbol/BaseSymbol.h"

namespace Engine
{
class Win32Symbol : public BaseSymbol
{
private:
	std::wstring m_bufferModulePath;
	std::vector<ModuleHandle> m_bufferModule{ 256 };

	// try using kernel32 api
	_NODISCARD bool K32RetrieveModuleList() noexcept;

	// try using tlhelp32 api
	_NODISCARD bool Tlhelp32RetrieveModuleList() noexcept;

public:
	Win32Symbol(const std::shared_ptr<ProcessData>& target) EXCEPT;
	std::vector<ProcessModule>& Update() EXCEPT override;

	// format and address into module.function+0xxxx
	_NODISCARD bool AddressToSymbol(POINTER address, std::string& symbol, size_t size = 512) const noexcept override;

	// return an adddress from a symbol such as module.function+0xxxx
	_NODISCARD POINTER SymbolToAddress(const std::string& symbol) const noexcept override;
};

}
#endif // !TOKIO_ENGINE_SYMBOL_WIN32SYMBOL_H