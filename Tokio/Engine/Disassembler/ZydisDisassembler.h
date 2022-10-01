#include "Engine/EngineDef.hpp"
#include "Engine/Disassembler/BaseDisassembler.hpp"

typedef struct ZydisDecoder_ ZydisDecoder;


namespace Engine
{

class ZydisDisassembler : public BaseDisassembler
{
private:
	ZydisDecoder* m_decoder = nullptr;
public:

	ZydisDisassembler();

	// pVirtualBase : The starting point of the memory in the target process
	// pOpCodes		: Pointer to the opcode buffer to be disassemble
	// size			: Size of the buffer
	virtual auto Disasm(POINTER pVirtualBase, const BYTE* pOpCodes, size_t size)->SafeResult(std::vector<DisasmData>) override;
};
}