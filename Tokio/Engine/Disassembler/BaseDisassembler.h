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

#ifndef TOKIO_ENGINE_BASEDISASSEMBLER_H
#define TOKIO_ENGINE_BASEDISASSEMBLER_H

#include "Engine/EngineDef.h"
#include "Common/Exception.h"
#include <memory>

namespace Engine
{

class BaseDisassembler
{
protected:
	std::shared_ptr<ProcessData> m_target;
public:
	BaseDisassembler(const std::shared_ptr<ProcessData>& target);

	// virtualAddress : The starting point of the memory in the target process
	// pOpCodes		  : Pointer to the opcode buffer to be disassemble
	// size			  : Size of the buffer
	_NODISCARD virtual std::vector<DisasmInstruction>
	Disassemble(POINTER virtualAddress, const byte_t* pOpCodes, size_t size) const EXCEPT = 0;

	// The user interface will call this function to refresh the new settings
	// any inherited disassembler of this class should handle settings changes
	virtual void UpdateSettings() noexcept;
};
}

#endif // !TOKIO_ENGINE_BASEDISASSEMBLER_HPP