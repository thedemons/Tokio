#pragma once
#include "stdafx.h"
#include "TokioAnalyzer.h"
#include "Settings.h"

#include "Engine/EngineAnalyzerDef.h"
#include "Engine/Memory/BaseMemory.h"
#include "Engine/Symbol/BaseSymbol.h"
#include "Engine/Disassembler/BaseDisassembler.h"

using namespace std::string_literals;

namespace Engine
{

ImGui::TokenizedText FormatSymbolAddress(
	POINTER address,
	const ResultGetSymbol& resultGetSymbol,
	bool* isAtBaseModule = nullptr,
	bool* isAtSubroutineStart = nullptr
)
{
	static const auto& settings = Settings::theme.disasm;

	// parse the address normally if there are no symbol
	if (!resultGetSymbol.has_value())
	{
		return ImGui::TokenizedText(settings.AddressAbs, "%llX", address);
	}

	// if it doesn't have symbol, it must belongs to a module
	if (!resultGetSymbol.has_symbol())
	{
		auto pModule = resultGetSymbol.Module();

		// if it isn't exactly at the module base address
		if (address != pModule->base)
		{
			// format: module.dll+8C
			ImGui::TokenizedText symbol(3);
			symbol.push_back(pModule->moduleNameA, settings.Module);
			symbol.push_back("+"s, settings.Delimeter);
			symbol.push_back(settings.Displacement, "%llX", address - pModule->base);

			if (isAtBaseModule) *isAtBaseModule = false;
			if (isAtSubroutineStart) *isAtSubroutineStart = false;

			return symbol;
		}
		else
		{
			// format: module.dll
			if (isAtBaseModule) *isAtBaseModule = true;
			if (isAtSubroutineStart) *isAtSubroutineStart = false;
			return ImGui::TokenizedText(pModule->moduleNameA, settings.Module);
		}
	}
	else
	{
		auto pModule = resultGetSymbol.Module();
		auto pSymbol = resultGetSymbol.Symbol();

		POINTER offsetFromVA = address - (pModule->base + pSymbol->offset);

		// if it isn't exactly at the starting address of the function
		if (offsetFromVA > 0)
		{
			// format: module.func+8C
			ImGui::TokenizedText symbol(5);
			symbol.push_back(pModule->moduleShortName, settings.Module);
			symbol.push_back("."s, settings.Delimeter);
			symbol.push_back(pSymbol->name, settings.Function);
			symbol.push_back("+"s, settings.Delimeter);
			symbol.push_back(settings.Displacement, "%llX", offsetFromVA);

			if (isAtBaseModule) *isAtBaseModule = false;
			if (isAtSubroutineStart) *isAtSubroutineStart = false;

			return symbol;
		}
		else
		{
			// format: module.func
			ImGui::TokenizedText symbol(3);
			symbol.push_back(pModule->moduleShortName, settings.Module);
			symbol.push_back("."s, settings.Delimeter);
			symbol.push_back(pSymbol->name, settings.Function);

			if (isAtBaseModule) *isAtBaseModule = false;
			if (isAtSubroutineStart) *isAtSubroutineStart = true;
			return symbol;
		}
	}
}



common::errcode TokioAnalyzer::AnalyzeRegion(
	const MemoryReadRegion& region,
	const std::vector<BYTE>& buffer,
	const size_t bufferOffset,
	size_t& instructionIndex,
	AnalyzedData& data
)
{
	auto disasmResult = m_disassembler->Disasm(region.start, buffer.data() + bufferOffset, region.size);
	if (disasmResult.has_error())
	{
		assert(false && "Disassemble failed?");
		return common::errcode::AnalyzerFailedToDisassemble;
	}

	std::vector<DisasmInstruction>& disasmInstructions = disasmResult.value();
	size_t insBufferOffset = bufferOffset;

	// init symbol walk context, this is for the address, which is in ascending order
	// it helps a lot with the performance to do this way, look at the function
	// if you ever forget how the fuck does it work
	auto walkContext = m_symbol->AddressSymbolWalkInit();

	// loop through all the disassembled instructions
	for (DisasmInstruction& disasmData : disasmInstructions)
	{
		AnalyzedInstruction& instruction = data.instructions[instructionIndex++];

		// color of the mnemonic base on its type
		DWORD mnemonicColor = Settings::GetDisasmColor(disasmData.mnemonic.type);

		// get the symbol data from this instruction address
		auto resultGetSymbol = m_symbol->AddressSymbolWalkNext(walkContext, disasmData.address);

		// copy the result from disasmData
		instruction.address           = disasmData.address;
		instruction.length            = disasmData.length;

		instruction.mnemonic          = std::move(disasmData.mnemonic);
		instruction.fmtMnemonic       = ImGui::TokenizedText(disasmData.mnemonic.value, mnemonicColor);

		instruction.bufferOffset      = insBufferOffset;

		instruction.referencedAddress = disasmData.referencedAddress;
		instruction.isRefPointer      = disasmData.isRefPointer;

		instruction.operands          = std::move(disasmData.operands);
		instruction.isNotReadable = false;

		// format the address for the instruction
		auto a = FormatSymbolAddress(
			disasmData.address,
			resultGetSymbol,
			&instruction.isAtBaseModule,
			&instruction.isAtSubroutineStart
		);
		instruction.fmtAddress.move(a);

		// clear the operands
		instruction.fmtOperand.clear();

		// here we parse the tokenized operands into colored text
		for (auto& operand : instruction.operands)
		{
			// skip the mnemonic and invalid operands (the mnemonic is already at the instruction.mnemonic)
			if (IsOperandMnemonic(operand.type) || operand.type == DisasmOperandType::Invalid) continue;

			if (operand.type == DisasmOperandType::WhiteSpace)
			{
				instruction.fmtOperand.push_back(" "s, 0x00000000);
			}
			// it has a reference address, find a symbol for it
			else if (instruction.referencedAddress != 0ull && operand.type == DisasmOperandType::AddressAbs)
			{
				auto resultGetRefSymbol = m_symbol->AddressToModuleSymbol(instruction.referencedAddress);
				instruction.fmtOperand += FormatSymbolAddress(instruction.referencedAddress, resultGetRefSymbol);
			}
			// it's just a regular operand
			else
			{
				DWORD operandColor = Settings::GetDisasmColor(operand.type);
				instruction.fmtOperand.push_back(operand.value, operandColor);
			}
		}

		// if it's is something like `mov rax, [0x12345]`
		//if (instruction.isRefPointer) AnalyzeInstructionReference(insData);


		insBufferOffset += instruction.length;
	}

	return {};
}

_NODISCARD common::errcode TokioAnalyzer::Analyze(
	POINTER address,
	size_t size,
	bool bDisectSubroutine,
	std::vector<BYTE>& outBuffer,
	AnalyzedData& outData
)
{
	UNUSED(bDisectSubroutine);

	if (size == 0)
	{
		assert(size != 0 && "The size to analyze was zero, what happened?");

		// IMPROVEMENT: return no error anyway, might change this in the future
		return {};
	}
	

	POINTER startAddress = address;
	size_t bufferSize = size;

	outBuffer.resize(size);
	std::vector<MemoryReadRegion> regions;
	m_memory->ReadMemSafe(startAddress, outBuffer.data(), bufferSize, regions);

	// invalidate all of the instructions
	AnalyzedInstruction invalidInstruction(outData);
	invalidInstruction.isNotReadable = true;
	outData.instructions.resize(size, invalidInstruction);

	// no readable memory, set all instructions to invalid
	if (regions.size() == 0)
	{
		for (auto& instruction : outData.instructions)
		{
			instruction.address = startAddress++;
			instruction.isNotReadable = true;
		}

		return common::errcode::AnalyzerFailedToReadMemory;
	}

	


	auto regionIter = regions.begin();

	// loop through the buffer, if it's not readable then push invalid instructions, else disassemble it
	size_t offset = startAddress;
	size_t instructionIndex = 0;

	while (offset < startAddress + bufferSize)
	{
		// the address was not readable
		if (offset < regionIter->start)
		{
			for (; offset < regionIter->start; offset++)

			//size_t unreadableSize = regionIter->start - offset;
			//instructionIndex += unreadableSize;

			//for (auto iter = outData.instructions.begin() + instructionIndex;
			//	offset < regionIter->start; iter++, offset++)
			{
				AnalyzedInstruction& ins = outData.instructions[instructionIndex++];
				ins.address = offset;
				ins.isNotReadable = true;
			}
		}
		// disassemble valid region
		else
		{
			auto resultAnalyzeRegion = AnalyzeRegion(*regionIter, outBuffer, regionIter->start - startAddress, instructionIndex, outData);
			if (resultAnalyzeRegion != common::errcode::Success)
			{
				//assert(false && "Failed to analyze region");
				return resultAnalyzeRegion;
			}

			offset += regionIter->size;
			if (++regionIter == regions.end()) break;
		}
	}

	// there are more bytes left after disassembling valid regions
	// push invalid instructions
	for (; offset < startAddress + bufferSize; offset++)
	{
		AnalyzedInstruction& ins = outData.instructions[instructionIndex++];
		ins.address = offset;
		ins.isNotReadable = true;
	}

	outData.instructions.resize(instructionIndex, invalidInstruction);
	//outData.subroutines.resize(0, outData);
	return common::errcode::Success;
}


}
