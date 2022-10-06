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
	Disassemble(POINTER virtualAddress, const BYTE* pOpCodes, size_t size) const EXCEPT = 0;

	// The user interface will call this function to refresh the new settings
	// any inherited disassembler of this class should handle settings changes
	virtual void UpdateSettings() noexcept;
};
}

#endif // !TOKIO_ENGINE_BASEDISASSEMBLER_HPP