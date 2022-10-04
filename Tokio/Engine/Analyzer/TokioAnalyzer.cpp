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

bool AnalyzeSubroutineBlocks(AnalyzedData& data, SubroutineInfo& subroutine, size_t start_index)
{
	AnalyzedInstruction& firstInstruction = data.instructions[start_index];
	
	// check if this instruction already has a block (already analyzed)
	if (firstInstruction.iBlock != UPTR_UNDEFINED) return true;

	SubroutineBlock* thisBlock = &subroutine.AddBlock();
	thisBlock->insStartIndex = start_index;

	size_t thisBlockIndex = subroutine.blocks.size();
	firstInstruction.iBlock = thisBlockIndex;

	for (size_t index = start_index; index < data.instructions.size() - 1; index++)
	{
		AnalyzedInstruction& instruction = data.instructions[index];
		AnalyzedInstruction& next_instruction = data.instructions[index + 1];

		// if this instruction is a "ret"
		if (instruction.mnemonic.type == DisasmOperandType::mneReturn)
		{
			thisBlock->insEndIndex = index + 1;
			return true;
		}
		// if this is an "int3"
		else if (instruction.mnemonic.type == DisasmOperandType::mneInt3)
		{
			thisBlock->insEndIndex = index + 1;
			return true;
		}
		// if the next instruction has a block (already analyzed)
		else if (next_instruction.iBlock != UPTR_UNDEFINED)
		{
			// we set this block next index to the next instruction block index
			thisBlock->iNextBlock = next_instruction.iBlock;
			thisBlock->insEndIndex = index + 1;
			return true;
		}

		// if the instruction was a jump without condition
		if (instruction.mnemonic.type == DisasmOperandType::mneJump)
		{
			thisBlock->insEndIndex = index + 1;

			// if the referenced index is valid
			if (instruction.referencedIndex != UPTR_UNDEFINED)
			{
				// jump out of the subroutine, we should mark it as ended here
				if (instruction.referencedIndex > index && next_instruction.mnemonic.type == DisasmOperandType::mneInt3) return true;


				// analyzed the block that this instruction jumps to
				thisBlock->iNextBlock = subroutine.blocks.size();
				return AnalyzeSubroutineBlocks(data, subroutine, instruction.referencedIndex);
			}

			// if the referenced index was invalid, there are multiple reasons why
			// it might jumps to somewhere outside of the analyzed range, or the opcode
			// was corrupted, or this simply isn't a subroutine
			return true;
		}
		// if is is a jump with condition, there are two paths
		else if (instruction.mnemonic.type == DisasmOperandType::mneJumpCondition)
		{
			thisBlock->insEndIndex = index + 1;

			// if the referenced index is valid, it's probably isn't valid if it points to
			// somewhere outside the analyzed range
			if (instruction.referencedIndex != UPTR_UNDEFINED)
			{
				// analyzed the block that this instruction jumps to
				thisBlock->iConditionalBlock = subroutine.blocks.size();
				bool result = AnalyzeSubroutineBlocks(data, subroutine, instruction.referencedIndex);

				if (!result) return false;
			}

			// notice that we don't use the pointer thisBlock here, because when we call
			// AnalyzeSubroutineBlocks again (recursively), it might have emplace_back() 
			// and thus make the thisBlock pointer invalid
			subroutine.blocks[thisBlockIndex].iNextBlock = subroutine.blocks.size();

			// continue analyze the next instruction
			return AnalyzeSubroutineBlocks(data, subroutine, index + 1);
		}
	}

	// the end point was not found, either the subroutine was too big that it ended outside
	// of the analyzed region, or simply isn't a subroutine
	return false;
}

void TokioAnalyzer::AnalyzeSubroutines(AnalyzedData& data)
{

	bool bLastInt3 = false;
	bool bLastUnreadable = false;

	for (size_t index = 0; index < data.instructions.size(); index++)
	{
		AnalyzedInstruction& instruction = data.instructions[index];
		bool bIsInt3 = instruction.mnemonic.type == DisasmOperandType::mneInt3;

		// case 1: check if the last instruction was int3 and this one is not
		// case 2: check if the last instruction was unreadable and this one is not
		// case 3: check if this instruction has a symbol and start at the offset 0 (isAtSubroutineStart)

		if ((bLastInt3 && !bIsInt3) || (bLastUnreadable && !instruction.isNotReadable) || instruction.isAtSubroutineStart)
		{
			SubroutineInfo& subroutine = data.AddSubroutine();
			if (AnalyzeSubroutineBlocks(data, subroutine, index) && subroutine.blocks.size() > 0)
			{
				// find the start and end point of the subroutine simply by min max the blocks start/end index
				subroutine.start = UPTR_UNDEFINED;
				subroutine.end = 0;
				size_t startidx = UPTR_UNDEFINED;
				size_t endidx = UPTR_UNDEFINED;

				for (auto& block : subroutine.blocks)
				{
					POINTER startAddr = data.instructions[block.insStartIndex].address;
					POINTER endAddr = data.instructions[block.insEndIndex].address;

					if (startAddr < subroutine.start)
					{
						subroutine.start = startAddr;
						startidx = block.insStartIndex;
					}
					if (endAddr > subroutine.end)
					{
						subroutine.end = endAddr;
						endidx = block.insEndIndex;
					}
				}

				data.instructions[startidx].isAtSubroutineStart = true;
				data.instructions[endidx].isAtSubroutineEnd = true;

				index = endidx + 1;

				bLastUnreadable = data.instructions[index].isNotReadable;
				bLastInt3 = bIsInt3;
				
				continue;
			}
			else
			{
				data.subroutines.pop_back();
			}
		}

		bLastInt3 = bIsInt3;
		bLastUnreadable = instruction.isNotReadable;
	}
}

void TokioAnalyzer::AnalyzeCrossReferences(AnalyzedData& data)
{
	static ImGui::TokenizedText xRefString("REF: "s, Settings::theme.disasm.Xref);
	
	POINTER instructionStart = data.instructions.front().address;
	POINTER instructionEnd = data.instructions.back().address;
	
	auto iterBegin = data.instructions.begin();
	auto iterEnd = data.instructions.end();

	// calculate the reference index, mainly used for jump and call pointer rendering
	for (auto it = iterBegin; it != iterEnd; it++)
	{
		// if the reference address is in range of the current instructions
		if (it->referencedAddress != 0 &&
			instructionStart <= it->referencedAddress &&
			it->referencedAddress <= instructionEnd)
		{

			std::vector<AnalyzedInstruction>::iterator reference;

			// we are optimizing the search by doing this in a sorted list
			// will binary search be better?
			bool bHasReference = false;
			if (it->referencedAddress > it->address)
			{
				reference = std::find(it, iterEnd, it->referencedAddress);
				bHasReference = reference != iterEnd;
			}
			else
			{
				reference = std::find(iterBegin, it, it->referencedAddress);
				bHasReference = reference != it;
			}


			if (bHasReference)
			{
				it->referencedInstruction = reference._Ptr;
				it->referencedIndex = reference - iterBegin;

				reference->referers.push_back(it._Ptr);

				reference->fmtComment.append_space(xRefString);
				reference->fmtComment += it->fmtAddress;

			}
			else
			{
				it->referencedAddress = 0;
				it->isRefPointer = false;
			}


		}
	}
}

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

		instruction.bufferOffset      = insBufferOffset;

		instruction.referencedAddress = disasmData.referencedAddress;
		instruction.isRefPointer      = disasmData.isRefPointer;

		instruction.operands          = std::move(disasmData.operands);
		instruction.isNotReadable = false;

		// format the address for the instruction
		//instruction.fmtMnemonic.clear();
		instruction.fmtMnemonic.push_back(disasmData.mnemonic.value, mnemonicColor);

		instruction.fmtAddress = std::move(FormatSymbolAddress(
			disasmData.address,
			resultGetSymbol,
			&instruction.isAtBaseModule,
			&instruction.isAtSubroutineStart
		));

		// clear all the formatted text
		//instruction.fmtOperand.clear();
		//instruction.fmtComment.clear();

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
	bool bAnalyzeSubroutine,
	std::vector<BYTE>& outBuffer,
	AnalyzedData& outData
)
{

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

	// FIXME-PERFORMANCE: Using clear is bad, reduce performance about 20%, but we had to do it
	outData.instructions.clear();
	outData.instructions.resize(size, invalidInstruction);


	// no readable memory, set all instructions to invalid
	if (regions.size() == 0)
	{
		for (auto& instruction : outData.instructions)
		{
			instruction.address = startAddress++;
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

	AnalyzeCrossReferences(outData);

	if (bAnalyzeSubroutine)
	{
		AnalyzeSubroutines(outData);

	}
	

	return common::errcode::Success;
}


}
