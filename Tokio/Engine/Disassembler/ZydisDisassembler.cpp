#include "stdafx.h"
#include "ZydisDisassembler.h"
#include "Engine/Engine.h"

#include <inttypes.h>
#include <Zycore/Format.h>
#include <Zycore/LibC.h>
#include <Zydis/Zydis.h>

#include "Settings.h"

namespace Engine
{

ZydisDisassembler::ZydisDisassembler(const std::shared_ptr<ProcessData>& target) : BaseDisassembler(target)
{
    g_SymbolEngine = Engine::Symbol();

    m_decoder32 = new ZydisDecoder;
    m_decoder64 = new ZydisDecoder;
    m_formatter = new ZydisFormatter;

    ZydisDecoderInit(m_decoder64, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
    ZydisDecoderInit(m_decoder32, ZYDIS_MACHINE_MODE_LONG_COMPAT_32, ZYDIS_STACK_WIDTH_32);
    
    InitFormatter();
    UpdateSettings();

    default_print_address_absolute = ZydisHookAddressFormatter;
    ZydisFormatterSetHook(
        m_formatter,
        ZYDIS_FORMATTER_FUNC_PRINT_ADDRESS_ABS,
        (const void**)&default_print_address_absolute
    );

}

void ZydisDisassembler::InitFormatter()
{
    auto& settings = Settings::disasm;
    ZydisFormatterStyle fmtStyle = ZYDIS_FORMATTER_STYLE_INTEL;
    if (settings.style == Settings::Disasm::FormatStyle::ATT)
        fmtStyle = ZYDIS_FORMATTER_STYLE_ATT;
    else if (settings.style == Settings::Disasm::FormatStyle::MASM)
        fmtStyle = ZYDIS_FORMATTER_STYLE_INTEL_MASM;


    ZydisFormatterInit(m_formatter, fmtStyle);
}

void ZydisDisassembler::UpdateSettings()
{
    auto& settings = Settings::disasm;

    ZyanUPointer paddingStyle = settings.bAutoHexPadding ? ZYDIS_PADDING_AUTO : ZYDIS_PADDING_DISABLED;

    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_FORCE_SEGMENT           , settings.bShowOperanSegment);
    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_FORCE_SIZE              , settings.bShowOperandSize);
    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_DETAILED_PREFIXES       , settings.bShowDetailedPerfix);

    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_UPPERCASE_MNEMONIC      , settings.bUppercaseMnemonic);
    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_UPPERCASE_REGISTERS     , settings.bUppercaseRegister);
    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_UPPERCASE_TYPECASTS     , settings.bUppercaseTypeCast);
    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_UPPERCASE_DECORATORS    , settings.bUppercaseDecorator);
    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_HEX_UPPERCASE           , settings.bUppercaseHex);
    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_HEX_FORCE_LEADING_NUMBER, settings.bShowHexLeadingZero);

    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_DEC_PREFIX              , reinterpret_cast<ZyanUPointer>(settings.cDecPrefix));
    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_DEC_SUFFIX              , reinterpret_cast<ZyanUPointer>(settings.cDecSuffix));
    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_HEX_PREFIX              , reinterpret_cast<ZyanUPointer>(settings.cHexPrefix));
    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_HEX_SUFFIX              , reinterpret_cast<ZyanUPointer>(settings.cHexSuffix));

    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_ADDR_PADDING_ABSOLUTE   , paddingStyle);
    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_ADDR_PADDING_RELATIVE   , paddingStyle);
    //ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_DISP_PADDING            , paddingStyle); // CAUSE SOME BUG IN ZYDIS TO FAIL TOKENIZE THE INSTRUCTION
    ZydisFormatterSetProperty(m_formatter, ZYDIS_FORMATTER_PROP_IMM_PADDING             , paddingStyle);
}

ZydisDisassembler::~ZydisDisassembler()
{
    delete m_decoder64;
    delete m_decoder32;
    delete m_formatter;
}

struct ZydisCallbackUserData
{
    ZydisDisassembler* pThis      { nullptr };
    DisasmInstruction* disasmData { nullptr };
    ZyanU8 id_refaddress          {   204u  }; // 204 is the default for unused operand, i don't know why
};

DisasmOperandType ZydisDisassembler::GetOperandMnemonicType(ZydisMnemonic mnemonic) const noexcept
{
    switch (mnemonic)
    {
        case ZYDIS_MNEMONIC_CALL: return  DisasmOperandType::mneCall;
        case ZYDIS_MNEMONIC_SYSCALL: return  DisasmOperandType::mneSyscall;
        case ZYDIS_MNEMONIC_RET: return  DisasmOperandType::mneReturn;
        case ZYDIS_MNEMONIC_JMP: return  DisasmOperandType::mneJump;
        case ZYDIS_MNEMONIC_INT3: return  DisasmOperandType::mneInt3;

        case ZYDIS_MNEMONIC_JB:
        case ZYDIS_MNEMONIC_JBE:
        case ZYDIS_MNEMONIC_JCXZ:
        case ZYDIS_MNEMONIC_JECXZ:
        case ZYDIS_MNEMONIC_JKNZD:
        case ZYDIS_MNEMONIC_JKZD:
        case ZYDIS_MNEMONIC_JL:
        case ZYDIS_MNEMONIC_JLE:
        case ZYDIS_MNEMONIC_JNB:
        case ZYDIS_MNEMONIC_JNBE:
        case ZYDIS_MNEMONIC_JNL:
        case ZYDIS_MNEMONIC_JNLE:
        case ZYDIS_MNEMONIC_JNO:
        case ZYDIS_MNEMONIC_JNP:
        case ZYDIS_MNEMONIC_JNS:
        case ZYDIS_MNEMONIC_JNZ:
        case ZYDIS_MNEMONIC_JO:
        case ZYDIS_MNEMONIC_JP:
        case ZYDIS_MNEMONIC_JRCXZ:
        case ZYDIS_MNEMONIC_JS:
        case ZYDIS_MNEMONIC_JZ: 
        case ZYDIS_MNEMONIC_LOOP: 
        case ZYDIS_MNEMONIC_LOOPE:
        case ZYDIS_MNEMONIC_LOOPNE: 
            return DisasmOperandType::mneJumpCondition;
    }

    return DisasmOperandType::Mnemonic;
}

ZyanStatus ZydisDisassembler::ZydisHookAddressFormatter(
    const ZydisFormatter* formatter,
    ZydisFormatterBuffer* buffer,
    ZydisFormatterContext* context
)
{
    ZydisCallbackUserData* UserData = static_cast<ZydisCallbackUserData*>(context->user_data);

    ZyanU64 address;
    ZYAN_CHECK(ZydisCalcAbsoluteAddress(context->instruction, context->operand,
        context->runtime_address, &address));

    UserData->disasmData->referencedAddress = address;
    UserData->id_refaddress = context->operand->id;


    return UserData->pThis->default_print_address_absolute(formatter, buffer, context);
}

_NODISCARD auto ZydisDisassembler::Disasm(POINTER pVirtualBase, const BYTE* pOpCodes, size_t size)->SafeResult(std::vector<DisasmInstruction>)
{

    static char buffer[256];

    // select the right decoder
    ZydisDecoder* decoder = m_target->is32bit ? m_decoder32 : m_decoder64;

    // first we decode the buffer to instructions
    ZydisDecodedInstruction instruction;

    // then tokenize it
    const ZydisFormatterToken* token;

    // after tokenizing it we can get the results
    ZydisTokenType token_type;

    // token value is const char*
    ZyanConstCharPointer token_value = nullptr;

    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

    std::vector<DisasmInstruction> result;
    result.resize(size);
    
    size_t instructionIndex = 0;

    POINTER pVirtualPointer = pVirtualBase;
    POINTER pVirtualEnd = pVirtualBase + size;

    while (pVirtualPointer < pVirtualEnd)
    {
        DisasmInstruction& disasmData = result[instructionIndex++];
        disasmData.address = pVirtualPointer;

        ZyanStatus zResultDecode = ZydisDecoderDecodeFull(decoder, pOpCodes, size, &instruction, operands,
            ZYDIS_MAX_OPERAND_COUNT, 0);

        // the operation didn't success
        // leave an "invalid"
        if (!ZYAN_SUCCESS(zResultDecode)) 
        {
            auto& operand = disasmData.operands.emplace_back();
            operand.type = DisasmOperandType::Invalid;

            memcpy_s(operand.value, sizeof(operand.value), "invalid", 8);

            disasmData.mnemonic = operand;
            disasmData.length = 1ull;
        }
        else
        {
            disasmData.length = instruction.length;

            // setup userdata
            ZydisCallbackUserData UserData{ this, &disasmData };

            // tokenize operands
            ZyanStatus zResultTokenize = 
                ZydisFormatterTokenizeInstruction(
                    m_formatter, 
                    &instruction, 
                    operands,
                    instruction.operand_count_visible,
                    buffer,
                    sizeof(buffer),
                    pVirtualPointer,
                    &token,
                    static_cast<void*>(&UserData) // UserData pass to ZydisHookAddressFormatter
                );

            if (ZYAN_SUCCESS(zResultTokenize))
            {
                disasmData.operands.resize(20);
                size_t operandIndex = 0;

                // get tokenized operands
                while (true)
                {
                    auto& operand = disasmData.operands[operandIndex++];

                    ZydisFormatterTokenGetValue(token, &token_type, &token_value);

                    if (token_value != nullptr)
                    {
                        size_t token_len = strnlen_s(token_value, sizeof(operand.value));
                        memcpy_s(operand.value, sizeof(operand.value), token_value, token_len);

                        // if this is the mnemonic operand
                        if (token_type == ZYDIS_TOKEN_MNEMONIC)
                        {
                            operand.type = GetOperandMnemonicType(instruction.mnemonic);
                            disasmData.mnemonic = operand;
                        }
                        else
                        {
                            operand.type = GetOperandType(token_type);

                            // check for reference address is a pointer
                            if (token_type == ZYDIS_TOKEN_PARENTHESIS_CLOSE && disasmData.operands.size() >= 2)
                            {
                                size_t index = disasmData.operands.size() - 1;
                                if ((disasmData.operands[index - 1].type == DisasmOperandType::AddressAbs) &&
                                    (disasmData.operands[index - 2].type == DisasmOperandType::ParenthesisOpen))
                                {
                                    disasmData.isRefPointer = true;
                                }
                            }
                        }
                    }

                    if (!ZYAN_SUCCESS(ZydisFormatterTokenNext(&token))) break;
                }

                disasmData.operands.resize(operandIndex);
                //if (UserData.id_refaddress != 204u && UserData.id_refaddress < disasmData.operands.size())
                //{
                //    disasmData.operands[UserData.id_refaddress].refAddress = disasmData.refAddress;
                //}
            }
            else
            {
                assert(false && "Failed to tokenize the instructions");

                auto& operand = disasmData.operands.emplace_back();
                operand.type = DisasmOperandType::Invalid;

                memcpy_s(operand.value, sizeof(operand.value), "error", 6);

                disasmData.mnemonic = operand;
                //disasmData.length = 1ull; // don't change the length
            }
        }

        size -= disasmData.length;
        pOpCodes += disasmData.length;
        pVirtualPointer += disasmData.length;
    }
    

    result.resize(instructionIndex);
    return result;
}
}