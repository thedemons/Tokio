#ifndef TOKYO_ENGINE_DISASSEMBLER_ZYDISDISASSEMBLER_H
#define TOKYO_ENGINE_DISASSEMBLER_ZYDISDISASSEMBLER_H

#include "Engine/Disassembler/BaseDisassembler.h"
#include "Engine/Symbol/BaseSymbol.h"

// The only reason we do this and not actually including zydis
// headers is we don't want its declarations to be leaked into our
// Engine.cpp and other files
#if 0
#define ZYDIS_TOKEN_INVALID             0x00
#define ZYDIS_TOKEN_WHITESPACE          0x01
#define ZYDIS_TOKEN_DELIMITER           0x02
#define ZYDIS_TOKEN_PARENTHESIS_OPEN    0x03
#define ZYDIS_TOKEN_PARENTHESIS_CLOSE   0x04
#define ZYDIS_TOKEN_PREFIX              0x05
#define ZYDIS_TOKEN_MNEMONIC            0x06
#define ZYDIS_TOKEN_REGISTER            0x07
#define ZYDIS_TOKEN_ADDRESS_ABS         0x08
#define ZYDIS_TOKEN_ADDRESS_REL         0x09
#define ZYDIS_TOKEN_DISPLACEMENT        0x0A
#define ZYDIS_TOKEN_IMMEDIATE           0x0B
#define ZYDIS_TOKEN_TYPECAST            0x0C
#define ZYDIS_TOKEN_DECORATOR           0x0D
#define ZYDIS_TOKEN_SYMBOL              0x0E
#endif

// all the comments have been removed for a more compact code
// take look at them in the zydis library to know what is it for
extern "C" {
	typedef uint8_t ZyanU8;
	typedef uint32_t  ZyanU32;
	typedef ZyanU32 ZyanStatus;
	typedef ZyanU8 ZydisTokenType;
#pragma warning (push, 3)
	typedef enum ZydisMnemonic_ ZydisMnemonic;
#pragma warning (pop)
	typedef struct ZydisDecoder_ ZydisDecoder;
	typedef struct ZydisFormatter_ ZydisFormatter;
	typedef struct ZydisFormatterContext_ ZydisFormatterContext;
	typedef struct ZydisFormatterBuffer_ ZydisFormatterBuffer;
	typedef ZyanStatus(*ZydisFormatterFunc)(const ZydisFormatter* formatter,
		ZydisFormatterBuffer* buffer, ZydisFormatterContext* context);
}

namespace Engine
{

class ZydisDisassembler : public BaseDisassembler
{
private:

	ZydisDecoder* m_decoder64 = nullptr;
	ZydisDecoder* m_decoder32 = nullptr;
	ZydisFormatter* m_formatter = nullptr;

	ZydisFormatterFunc default_print_address_absolute;

	std::shared_ptr<BaseSymbol> g_SymbolEngine = nullptr;

	const std::array< DisasmOperandType, 15> m_arrayTokenType = {
		 DisasmOperandType::Invalid,
		 DisasmOperandType::WhiteSpace,
		 DisasmOperandType::Delimeter,
		 DisasmOperandType::ParenthesisOpen,
		 DisasmOperandType::ParenthesisClose,
		 DisasmOperandType::Prefix,
		 DisasmOperandType::Mnemonic,
		 DisasmOperandType::Register,
		 DisasmOperandType::AddressAbs,
		 DisasmOperandType::AddressRel,
		 DisasmOperandType::Displacement,
		 DisasmOperandType::Immediate,
		 DisasmOperandType::TypeCast,
		 DisasmOperandType::Decorator,
		 DisasmOperandType::Literal,
	};

	_NODISCARD _CONSTEXPR20 DisasmOperandType GetOperandType(ZydisTokenType tokenType) const noexcept
	{
		assert(tokenType >= 0 && tokenType < 15 );
		return m_arrayTokenType[tokenType];
	}

	_NODISCARD DisasmOperandType GetOperandMnemonicType(ZydisMnemonic mnemonic) const noexcept;

	static ZyanStatus ZydisHookAddressFormatter(const ZydisFormatter* formatter,
		ZydisFormatterBuffer* buffer, ZydisFormatterContext* context);

	void InitFormatter();
public:

	ZydisDisassembler(const std::shared_ptr<ProcessData>& target);
	~ZydisDisassembler();

	// virtualAddress : The starting point of the memory in the target process
	// pOpCodes		  : Pointer to the opcode buffer to be disassemble
	// size			  : Size of the buffer
	_NODISCARD std::vector<DisasmInstruction>
	Disassemble(POINTER virtualAddress, const BYTE* pOpCodes, size_t size) const EXCEPT override;

	void UpdateSettings() noexcept override;
};
}

#endif // !TOKYO_ENGINE_DISASSEMBLER_ZYDISDISASSEMBLER_H