#ifndef TOKIO_ENGINE_BASEDISASSEMBLER_H
#define TOKIO_ENGINE_BASEDISASSEMBLER_H

#include "Engine/EngineDef.h"
#include "common_result.hpp"

#include <memory>

namespace Engine
{

class BaseDisassembler
{
protected:
	std::shared_ptr<ProcessData> m_target;
public:
	BaseDisassembler(const std::shared_ptr<ProcessData>& target);

	// pVirtualBase : The starting point of the memory in the target process
	// pOpCodes		: Pointer to the opcode buffer to be disassemble
	// size			: Size of the buffer
	_NODISCARD virtual auto Disasm(POINTER pVirtualBase, const BYTE* pOpCodes, size_t size)->SafeResult(std::vector<DisasmInstruction>) = 0;

	// The user interface will call this function to refresh the new settings
	// any inherited disassembler of this class should handle settings changes
	virtual void UpdateSettings();
};
}

#endif // !TOKIO_ENGINE_BASEDISASSEMBLER_HPP