#include "stdafx.h"
#include "ZydisDisassembler.h"

#include <inttypes.h>
#include <Zycore/Format.h>
#include <Zycore/LibC.h>
#include <Zydis/Zydis.h>

namespace Engine
{

ZydisDisassembler::ZydisDisassembler()
{
    ZydisDecoderInit(&m_decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
}

_NODISCARD auto ZydisDisassembler::Disasm(POINTER pVirtualBase, const BYTE* pOpCodes, size_t size)->SafeResult(std::vector<DisasmData>)
{
	//RESULT_THROW(NoMessage);
    ZydisFormatter formatter;
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);
    ZydisFormatterSetProperty(&formatter, ZYDIS_FORMATTER_PROP_FORCE_SEGMENT, ZYAN_TRUE);
    ZydisFormatterSetProperty(&formatter, ZYDIS_FORMATTER_PROP_FORCE_SIZE, ZYAN_TRUE);

    // Replace the `ZYDIS_FORMATTER_FUNC_PRINT_ADDRESS_ABS` function that formats the absolute
    // addresses
    //default_print_address_absolute = (ZydisFormatterFunc)&ZydisFormatterPrintAddressAbsolute;
    //ZydisFormatterSetHook(&formatter, ZYDIS_FORMATTER_FUNC_PRINT_ADDRESS_ABS,
    //    (const void**)&default_print_address_absolute);

    ZyanU64 runtime_address = pVirtualBase;

    ZydisDecodedInstruction instruction;
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT_VISIBLE];
    char buffer[256];

    std::vector< DisasmData> result;

    ZyanU64 end_address = runtime_address + size;
    while (runtime_address < end_address)
    {
        bool isok = ZYAN_SUCCESS(ZydisDecoderDecodeFull(&m_decoder, pOpCodes, size, &instruction, operands,
            ZYDIS_MAX_OPERAND_COUNT_VISIBLE, ZYDIS_DFLAG_VISIBLE_OPERANDS_ONLY));

        if (!isok)
        {
            pOpCodes += 1;
            runtime_address += 1;
            continue;
        }

        DisasmData& disasmData = result.emplace_back();

        disasmData.address = runtime_address;
        disasmData.addressFormat = std::to_string(runtime_address);

        // We have to pass a `runtime_address` different to `ZYDIS_RUNTIME_ADDRESS_NONE` to
        // enable printing of absolute addresses
        ZydisFormatterFormatInstruction(&formatter, &instruction, operands,
            instruction.operand_count_visible, &buffer[0], sizeof(buffer), runtime_address,
            ZYAN_NULL);

        disasmData.instruction = buffer;

        pOpCodes += instruction.length;
        size -= instruction.length;
        runtime_address += instruction.length;
    }

    return result;
}
}