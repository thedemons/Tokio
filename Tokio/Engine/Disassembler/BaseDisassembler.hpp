#ifndef TOKIO_ENGINE_BASEDISASSEMBLER_HPP
#define TOKIO_ENGINE_BASEDISASSEMBLER_HPP

namespace Engine
{

class BaseDisassembler
{
public:
	// pVirtualBase : The starting point of the memory in the target process
	// pOpCodes		: Pointer to the opcode buffer to be disassemble
	// size			: Size of the buffer
	_NODISCARD virtual auto Disasm(POINTER pVirtualBase, const BYTE* pOpCodes, size_t size)->SafeResult(std::vector<DisasmData>) = 0;
};
}

#endif //  TOKIO_ENGINE_BASEDISASSEMBLER_HPP