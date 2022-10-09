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