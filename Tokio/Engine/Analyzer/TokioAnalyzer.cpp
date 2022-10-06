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

bool AnalyzeSubroutineBlocks(
	AnalyzedData& data,
	SubroutineInfo& subroutine,
	size_t start_index,
	size_t prevCondBlockIndex = UPTR_UNDEFINED,
	size_t prevBlockIndex = UPTR_UNDEFINED
)
{
	AnalyzedInstruction& firstInstruction = data.instructions[start_index];
	
	// check if this instruction already has a block (already analyzed)
	if (firstInstruction.blockIndex != UPTR_UNDEFINED)
	{
		// this instruction belongs to another subroutine
		if (firstInstruction.iSubroutine != UPTR_UNDEFINED)
		{
			// remove the linkage of the previous instruction
			if (prevBlockIndex != UPTR_UNDEFINED)
			{
				SubroutineBlock& previousBlock = subroutine.blocks[prevBlockIndex];
				previousBlock.nextBlockIndex = UPTR_UNDEFINED;
				
			}
			if (prevCondBlockIndex != UPTR_UNDEFINED)
			{
				SubroutineBlock& previousBlock = subroutine.blocks[prevCondBlockIndex];
				previousBlock.nextCondBlockIndex = UPTR_UNDEFINED;
				
			}
		}
		else
		{
			SubroutineBlock& thisBlock = subroutine.blocks[firstInstruction.blockIndex];

			if (thisBlock.prevBlockIndex == UPTR_UNDEFINED)
				thisBlock.prevBlockIndex = prevBlockIndex;

			if (thisBlock.prevCondBlockIndex == UPTR_UNDEFINED)
				thisBlock.prevCondBlockIndex = prevCondBlockIndex;
		}

		return true;
	}

	size_t thisBlockIndex = subroutine.blocks.size();
	SubroutineBlock& thisBlock = subroutine.AddBlock();

	thisBlock.instructionIndex = start_index;
	thisBlock.prevBlockIndex = prevBlockIndex;
	thisBlock.prevCondBlockIndex = prevCondBlockIndex;

	size_t index = start_index;

	for (; index < data.instructions.size() - 1; index++)
	{
		AnalyzedInstruction& instruction = data.instructions[index];
		AnalyzedInstruction& next_instruction = data.instructions[index + 1];

		bool bIsInvalid =
			instruction.mnemonic.type == DisasmOperandType::mneInt3 ||
			instruction.mnemonic.type == DisasmOperandType::Invalid ||
			instruction.isNotReadable;

		instruction.blockIndex = thisBlockIndex;

		//printf("%llx:  ", instruction.address);
		//for (auto& op : instruction.operands)
		//{
		//	printf("%s", op.value);
		//}
		//printf("\n");
		
		// if this instruction is a "ret"
		if (instruction.mnemonic.type == DisasmOperandType::mneReturn)
		{
			thisBlock.instructionCount = index - start_index + 1;
			return true;
		}
		else if (bIsInvalid)
		{
			thisBlock.instructionCount = index - start_index + 1;
			return false;
		}
		else if (next_instruction.iSubroutine != UPTR_UNDEFINED) 
		{
			thisBlock.instructionCount = index - start_index + 1;
			return false;
		}

		// if the instruction was a jump without condition
		if (instruction.mnemonic.type == DisasmOperandType::mneJump)
		{
			thisBlock.instructionCount = index - start_index + 1;

			// if the referenced index is valid
			if (instruction.referencedIndex != UPTR_UNDEFINED)
			{
				// jump out of the subroutine, we should mark it as ended here
				// FIXME <-- i added this FIXME a while ago and didn't remember what to fix..

				bool bNextInvalid =
					next_instruction.mnemonic.type == DisasmOperandType::mneInt3 ||
					next_instruction.mnemonic.type == DisasmOperandType::Invalid ||
					next_instruction.isNotReadable;

				if (instruction.referencedIndex > index && bNextInvalid) return true;

				// analyzed the block that this instruction jumps to
				thisBlock.nextBlockIndex = subroutine.blocks.size();
				return AnalyzeSubroutineBlocks(data, subroutine, instruction.referencedIndex, UPTR_UNDEFINED, thisBlockIndex);
			}

			// if the referenced index was invalid, there are multiple reasons why
			// it might jumps to somewhere outside of the analyzed range, or the opcode
			// was corrupted, or this simply isn't a subroutine
			return true;
		}
		// if is is a jump with condition, there are two paths
		else if (instruction.mnemonic.type == DisasmOperandType::mneJumpCondition)
		{
			thisBlock.instructionCount = index - start_index + 1;

			// if the referenced index is valid, it's probably isn't valid if it points to
			// somewhere outside the analyzed range
			if (instruction.referencedIndex != UPTR_UNDEFINED)
			{
				bool bNextInvalid =
					next_instruction.mnemonic.type == DisasmOperandType::mneInt3 ||
					next_instruction.mnemonic.type == DisasmOperandType::Invalid ||
					next_instruction.isNotReadable;

				// a conditional jump but its next instruction is invalid, this is not a subroutine
				if (bNextInvalid) break;

				// analyzed the block that this instruction jumps to
				thisBlock.nextCondBlockIndex = subroutine.blocks.size();
				bool result = AnalyzeSubroutineBlocks(data, subroutine, instruction.referencedIndex, thisBlockIndex);

				// continue analyze the next instruction if everything is fine
				if (!result) return false;
			}

			// don't analyze the next instruction if it was already analyzed
			if (next_instruction.blockIndex != UPTR_UNDEFINED)
			{
				// we set this block next index to the next instruction block index
				thisBlock.nextBlockIndex = next_instruction.blockIndex;
				subroutine.blocks[next_instruction.blockIndex].prevBlockIndex = thisBlockIndex;
				return true;
			}
			else
			{
				// notice that we don't use the pointer thisBlock here, because when we call
				// AnalyzeSubroutineBlocks again (recursively), it might have emplace_back() 
				// and thus make the thisBlock pointer invalid
				subroutine.blocks[thisBlockIndex].nextBlockIndex = subroutine.blocks.size();

				// continue analyze the next instruction
				return AnalyzeSubroutineBlocks(data, subroutine, index + 1, UPTR_UNDEFINED, thisBlockIndex);
			}
		}
		// or if it has any referer, i don't know how well this will work since other
		// instructions outside the subroutine could refer to it, might need rework. FIXME!
		else if (next_instruction.referers.size() > 0)
		{
			// create a new block on the next index
			thisBlock.instructionCount = index - start_index + 1;
			thisBlock.nextBlockIndex = subroutine.blocks.size();
			return AnalyzeSubroutineBlocks(data, subroutine, index + 1, UPTR_UNDEFINED, thisBlockIndex);
		}
	}


	// the end point was not found, either the subroutine was too big that it ended outside
	// of the analyzed region, or simply isn't a subroutine

	thisBlock.instructionCount = data.instructions.size() - start_index;
	return false;
}

inline size_t InitSubroutine(AnalyzedData& data, SubroutineInfo& subroutine, size_t subroutineIndex)
{
	// find the start and end point of the subroutine simply by min max the blocks start/end index
	subroutine.start = UPTR_UNDEFINED;
	subroutine.end = 0;
	size_t startidx = UPTR_UNDEFINED;
	size_t endidx = UPTR_UNDEFINED;

	for (auto& block : subroutine.blocks)
	{
		size_t end_index = block.instructionIndex + block.instructionCount - 1;
		POINTER startAddr = data.instructions[block.instructionIndex].address;
		POINTER endAddr = data.instructions[end_index].address;

		if (startAddr < subroutine.start)
		{
			subroutine.start = startAddr;
			startidx = block.instructionIndex;
		}
		if (endAddr > subroutine.end)
		{
			subroutine.end = endAddr;
			endidx = end_index;
		}
	}

	data.instructions[startidx].isAtSubroutineStart = true;
	data.instructions[endidx].isAtSubroutineEnd = true;

	// assign the subroutine to those instructions
	for (size_t i = startidx; i <= endidx; i++)
		data.instructions[i].iSubroutine = subroutineIndex;

	return endidx;
}

// THIS IS A MESS FIXME-PLEASE
void TokioAnalyzer::AnalyzeSubroutines(AnalyzedData& data, const std::vector<BYTE>& buffer)
{

	bool bLastInt3 = false;
	bool bLastNull = false;
	bool bLastUnreadable = false;
	bool bLastSubroutineEnded = false;

	for (size_t index = 0; index < data.instructions.size(); index++)
	{
		AnalyzedInstruction& instruction = data.instructions[index];
		bool bIsInt3 = instruction.mnemonic.type == DisasmOperandType::mneInt3;

		bool bIsNull = true;
		for (size_t bufIndex = instruction.bufferOffset; bufIndex < instruction.bufferOffset + instruction.length; bufIndex++)
		{
			if (buffer[bufIndex] != 0x00)
			{
				bIsNull = false;
				break;
			}
		}

		// case 1: check if the last instruction was int3 and this one is not
		// case 2: check if the last instruction was null and this one is not
		// case 3: check if the last instruction was unreadable and this one is not
		// case 4: check if this instruction has a symbol and start at the offset 0 (isAtSubroutineStart)
		// case 5: this is the instruction right after the last subroutine, and this is not null, not int3 and readable
		//system("cls");

		bool case5 = bLastSubroutineEnded && !bIsInt3 && !bIsNull && !instruction.isNotReadable;
		if (case5 || (bLastInt3 && !bIsInt3) || (bLastNull && !bIsNull) || (bLastUnreadable && !instruction.isNotReadable) || instruction.isAtSubroutineStart)
		{
			size_t subroutineIndex = data.subroutines.size();
			SubroutineInfo& subroutine = data.AddSubroutine();


			if (AnalyzeSubroutineBlocks(data, subroutine, index) && subroutine.blocks.size() > 0)
			{
				bool bDestroySubroutine = false;

				// if it only has one block, contains one single instruction, and that instruction is "ret"
				// probably an invalid subroutine
				if (subroutine.blocks.size() == 1)
				{
					const SubroutineBlock& firstBlock = subroutine.blocks[0];
					if (firstBlock.instructionCount == 1)
					{
						const AnalyzedInstruction& ins = data.instructions[firstBlock.instructionIndex];
						bDestroySubroutine = ins.mnemonic.type == DisasmOperandType::mneReturn;
					}
				}

				if (!bDestroySubroutine)
				{
					index = InitSubroutine(data, subroutine, subroutineIndex);

					// FIXME: THIS IS SO FUCKING DIRTY
					bLastUnreadable = false;
					bLastInt3 = false;
					bLastNull = false;
					bLastSubroutineEnded = true;
					continue;
				}
			}

			// we must clear the block assignment if this is not a subroutine
			for (auto& block : subroutine.blocks)
			{
				for (size_t insIndex = block.instructionIndex; insIndex < block.instructionIndex + block.instructionCount; insIndex++)
				{
					auto& ins = data.instructions[insIndex];
					ins.blockIndex = UPTR_UNDEFINED;
					ins.iSubroutine = UPTR_UNDEFINED;
				}
			}

			data.subroutines.pop_back();

			// false subroutine symbol
			instruction.isAtSubroutineStart = false;
		}

		bLastNull = bIsNull;
		bLastInt3 = bIsInt3;
		bLastUnreadable = instruction.isNotReadable;
		bLastSubroutineEnded = false;
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
	for (std::vector<AnalyzedInstruction>::iterator it = iterBegin; it != iterEnd; it++)
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
			symbol.push_back(pSymbol->name, settings.Symbol);
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
			symbol.push_back(pSymbol->name, settings.Symbol);

			if (isAtBaseModule) *isAtBaseModule = false;
			if (isAtSubroutineStart) *isAtSubroutineStart = true;
			return symbol;
		}
	}
}



void TokioAnalyzer::AnalyzeRegion(
	const MemoryReadRegion& region,
	const std::vector<BYTE>& buffer,
	const size_t bufferOffset,
	size_t& instructionIndex,
	AnalyzedData& data
) EXCEPT
{
	try
	{	
		// disassemble the region first
		std::vector<DisasmInstruction> disasmInstructions = 
			m_disassembler->Disassemble(region.start, buffer.data() + bufferOffset, region.size);

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

	}
	catch (Tokio::Exception& e)
	{
		e += "TokioAnalyzer failed to analyed region";
		throw e;
	}
}

_NODISCARD void TokioAnalyzer::Analyze(
	POINTER address,
	size_t size,
	bool bAnalyzeSubroutine,
	std::vector<BYTE>& outBuffer,
	AnalyzedData& outData
) EXCEPT
{
	
	static const std::string except_zero("The size to analyze was zero");
	static const std::string except_read_failed("Failed to read memory");

	if (size == 0)
	{
		assert(size != 0 && "The size to analyze was zero, what happened?");
		throw Tokio::Exception(except_zero);
	}

	outBuffer.resize(size);


	std::vector<MemoryReadRegion> regions;
	m_memory->ReadMemSafe(address, outBuffer.data(), size, regions);

	// invalidate all of the instructions
	AnalyzedInstruction invalidInstruction(outData);
	invalidInstruction.isNotReadable = true;

	// FIXME-PERFORMANCE: Using clear is bad, reduce performance about 20%, but we had to do it
	outData.subroutines.clear();
	outData.instructions.clear();
	outData.instructions.resize(size, invalidInstruction);


	// no readable memory, set all instructions to invalid
	if (regions.size() == 0)
	{
		for (auto& instruction : outData.instructions)
		{
			instruction.address = address++;
		}

		throw Tokio::Exception(except_read_failed);
	}

	// loop through the buffer, if it's not readable then push invalid instructions, else disassemble it
	size_t offset = address;
	size_t instructionIndex = 0;

	auto regionIter = regions.begin();

	while (offset < address + size)
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
			AnalyzeRegion(*regionIter, outBuffer, regionIter->start - address, instructionIndex, outData);

			offset += regionIter->size;
			if (++regionIter == regions.end()) break;
		}
	}

	// there are more bytes left after disassembling valid regions
	// push invalid instructions
	for (; offset < address + size; offset++)
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
		AnalyzeSubroutines(outData, outBuffer);
	}
}


}
