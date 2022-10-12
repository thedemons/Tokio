/*
 * Project Tokio
 * Author: thedemons
 * Github: thedemons/Tokio
 * Made:   With love
 *
 * License: MIT License
 *
 * Copyright(c) 2022 thedemons
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#include "stdafx.h"
#include "TokioAnalyzer.h"
#include "Settings.h"

#include "Engine/EngineAnalyzerDef.h"
#include "Engine/Memory/BaseMemory.h"
#include "Engine/Symbol/BaseSymbol.h"
#include "Engine/Disassembler/BaseDisassembler.h"

#include "Common/StringHelper.h"

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
			if (firstInstruction.blockIndex >= subroutine.blocks.size())
			{
				Tokio::Log("Instruction block index out of bound?");
			}
			else
			{
				SubroutineBlock& thisBlock = subroutine.blocks[firstInstruction.blockIndex];

				if (thisBlock.prevBlockIndex == UPTR_UNDEFINED)
					thisBlock.prevBlockIndex = prevBlockIndex;

				if (thisBlock.prevCondBlockIndex == UPTR_UNDEFINED)
					thisBlock.prevCondBlockIndex = prevCondBlockIndex;
			}
		}

		return true;
	}
		//printf("new block\n");
	size_t thisBlockIndex = subroutine.blocks.size();

	if (thisBlockIndex > 1000)
	{
		Tokio::Log("Function too big at address: %llx\n", data.instructions[subroutine.blocks[0].instructionIndex].address);
		return false;
	}

	SubroutineBlock& thisBlock = subroutine.AddBlock();

	thisBlock.instructionIndex = start_index;
	thisBlock.prevBlockIndex = prevBlockIndex;
	thisBlock.prevCondBlockIndex = prevCondBlockIndex;

	size_t index = start_index;

	for (; index < data.instructions.size() - 1; index++)
	{
		AnalyzedInstruction& instruction = data.instructions[index];
		//if (instruction.address == 0x7ffe09c11741)
		//{
		//	printf("!23\n");
		//}
		AnalyzedInstruction& next_instruction = data.instructions[index + 1];

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

				// jump at the first instruction, this is a "forwarder" subroutine, return immediately
				// FIXME: might not be the case?
				if (thisBlockIndex == 0 && index == start_index)
				{
					return true;
				}
				bool bNextInvalid =
					next_instruction.mnemonic.type == DisasmOperandType::mneInt3 ||
					next_instruction.mnemonic.type == DisasmOperandType::Invalid ||
					next_instruction.isNotReadable;

				if (instruction.referencedIndex > index && bNextInvalid) return true;

				// analyzed the block that this instruction jumps to
				bool result = AnalyzeSubroutineBlocks(data, subroutine, instruction.referencedIndex, UPTR_UNDEFINED, thisBlockIndex);
				auto& refIns = data.instructions[instruction.referencedIndex];
				if (refIns.iSubroutine == UPTR_UNDEFINED)
					subroutine.blocks[thisBlockIndex].nextBlockIndex = refIns.blockIndex;
				return result;
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
				//bool bNextInvalid =
				//	next_instruction.mnemonic.type == DisasmOperandType::mneInt3 ||
				//	next_instruction.mnemonic.type == DisasmOperandType::Invalid ||
				//	next_instruction.isNotReadable;

				// FIXME: Edge case - still a subroutine
				// jnz 0x12345
				// int3
				// xor eax,eax
				//// a conditional jump but its next instruction is invalid, this is not a subroutine
				//if (bNextInvalid) return false;

				// analyzed the block that this instruction jumps to
				bool result = AnalyzeSubroutineBlocks(data, subroutine, instruction.referencedIndex, thisBlockIndex);

				auto& refIns = data.instructions[instruction.referencedIndex];
				if (refIns.iSubroutine == UPTR_UNDEFINED)
					subroutine.blocks[thisBlockIndex].nextCondBlockIndex = refIns.blockIndex;

				// continue analyze the next instruction if everything is fine
				if (!result) return false;
			}

			// don't analyze the next instruction if it was already analyzed
			if (next_instruction.blockIndex != UPTR_UNDEFINED)
			{
				// we set this block next index to the next instruction block index
				subroutine.blocks[thisBlockIndex].nextBlockIndex = next_instruction.blockIndex;
				subroutine.blocks[next_instruction.blockIndex].prevBlockIndex = thisBlockIndex;
				return true;
			}
			else
			{
				// continue analyze the next instruction
				bool result = AnalyzeSubroutineBlocks(data, subroutine, index + 1, UPTR_UNDEFINED, thisBlockIndex);

				// notice that we don't use the pointer thisBlock here, because when we call
				// AnalyzeSubroutineBlocks again (recursively), it might have emplace_back() 
				// and thus make the thisBlock pointer invalid
				subroutine.blocks[thisBlockIndex].nextBlockIndex = data.instructions[index + 1].blockIndex;
				return result;
			}
		}
		// the next instruction is already analyzed
		else if (next_instruction.blockIndex != UPTR_UNDEFINED)
		{
			thisBlock.instructionCount = index - start_index + 1;
			thisBlock.nextBlockIndex = next_instruction.blockIndex;
			subroutine.blocks[next_instruction.blockIndex].prevBlockIndex = thisBlockIndex;
			return true;
		}
		// or if it has any referer, i don't know how well this will work since other
		// instructions outside the subroutine could refer to it, might need rework. FIXME!
		else if (next_instruction.referers.size() > 0)
		{
			// create a new block on the next index
			thisBlock.instructionCount = index - start_index + 1;
			thisBlock.nextBlockIndex = subroutine.blocks.size();
			bool result = AnalyzeSubroutineBlocks(data, subroutine, index + 1, UPTR_UNDEFINED, thisBlockIndex);

			subroutine.blocks[thisBlockIndex].nextBlockIndex = data.instructions[index + 1].blockIndex;
			return result;
		}


		bool bNextIsInvalid =
			next_instruction.mnemonic.type == DisasmOperandType::mneInt3 ||
			next_instruction.mnemonic.type == DisasmOperandType::Invalid ||
			next_instruction.isNotReadable;

		if (bNextIsInvalid)
		{
			thisBlock.instructionCount = index - start_index + 1;
			return true;
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
	subroutine.address = UPTR_UNDEFINED;
	subroutine.size = 0;
	size_t startidx = UPTR_UNDEFINED;
	size_t endidx = UPTR_UNDEFINED;

	for (auto& block : subroutine.blocks)
	{
		size_t end_index = block.instructionIndex + block.instructionCount - 1;
		POINTER startAddr = data.instructions[block.instructionIndex].address;
		POINTER endAddr = data.instructions[end_index].address;

		if (subroutine.address == UPTR_UNDEFINED)
		// FIXME
		//if (startAddr < subroutine.address)
		{
			subroutine.address = startAddr;
			subroutine.instructionIndex = block.instructionIndex;
			startidx = block.instructionIndex;
		}
		if (endAddr > subroutine.size)
		if ((block.nextBlockIndex == UPTR_UNDEFINED && block.nextCondBlockIndex == UPTR_UNDEFINED) && endAddr > subroutine.size)
		{
			subroutine.size = endAddr;
			subroutine.instructionCount = end_index;
			endidx = end_index;
		}

		for (size_t index = block.instructionIndex; index < block.instructionIndex + block.instructionCount; index++)
		{
			data.instructions[index].iSubroutine = subroutineIndex;
		}
	}


	// could not find the subroutine end point
	if (endidx == UPTR_UNDEFINED)
	{
		return UPTR_UNDEFINED;
	}

	subroutine.size -= subroutine.address;
	subroutine.instructionCount -= subroutine.instructionIndex - 1;

	AnalyzedInstruction& startInstruction = data.instructions[startidx];

	startInstruction.isAtSubroutineStart = true;
	data.instructions[endidx].isAtSubroutineEnd = true;

	// assign the subroutine to those instructions
	//for (size_t i = startidx; i <= endidx; i++)
	//	data.instructions[i].iSubroutine = subroutineIndex;


	// if the size of the subroutine symbol doesn't match the subroutine
	if (startInstruction.symbolAddress.has_symbol())
	{
		POINTER subroutineOffset = subroutine.address - startInstruction.symbolAddress.Module()->base;
		ModuleSymbol* symbol = startInstruction.symbolAddress.Symbol();
		if (subroutineOffset == symbol->offset)
		{
			symbol->size = subroutine.size;
		}
	}

	return endidx;
}

// THIS IS A MESS FIXME-PLEASE
void TokioAnalyzer::AnalyzeSubroutines(AnalyzedData& data, size_t numInstructions, const std::vector<BYTE>& buffer)
{

	bool bLastInt3 = false;
	bool bLastNull = false;
	bool bLastUnreadable = false;
	bool bLastSubroutineEnded = false;

	for (size_t index = 0; index < numInstructions; index++)
	{
		AnalyzedInstruction& instruction = data.instructions[index];
		bool bIsInt3 = instruction.mnemonic.type == DisasmOperandType::mneInt3;
		if (bIsInt3)
		{
			bLastInt3 = true;
			continue;
		}
		if (instruction.iSubroutine != UPTR_UNDEFINED) continue;

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

		//bool case5 = bLastSubroutineEnded && !bIsInt3 && !bIsNull && !instruction.isNotReadable;
		//if (case5 || (bLastInt3) || (bLastNull && !bIsNull) || (bLastUnreadable && !instruction.isNotReadable) || instruction.isAtSubroutineStart)
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
					size_t endIndex = InitSubroutine(data, subroutine, subroutineIndex);
					if (endIndex != UPTR_UNDEFINED)
					{
						index = endIndex;

						// FIXME: THIS IS SO FUCKING DIRTY
						bLastUnreadable = false;
						bLastInt3 = false;
						bLastNull = false;
						bLastSubroutineEnded = true;
						continue;
					}
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
		bLastInt3 = false;
		bLastUnreadable = instruction.isNotReadable;
		bLastSubroutineEnded = false;
	}
}

void TokioAnalyzer::AnalyzeCrossReferences(AnalyzedData& data, size_t numInstructions) noexcept
{
	static ImGui::TokenizedText xRefString("REF: "s, Settings::theme.disasm.Xref);
	
	POINTER instructionStart = data.instructions.front().address;
	POINTER instructionEnd = data.instructions.back().address;
	
	auto iterBegin = data.instructions.begin();
	auto iterEnd = iterBegin + numInstructions;

	const size_t pointerSize = m_target->is32Bit() ? sizeof(dword_t) : sizeof(qword_t);

	// calculate the reference index, mainly used for jump and call pointer rendering
	for (std::vector<AnalyzedInstruction>::iterator it = iterBegin; it != iterEnd; it++)
	{
		// if the reference address is in range of the current instructions
		if (it->referencedAddress != 0)
		{

			if (
				instructionStart <= it->referencedAddress &&
				it->referencedAddress <= instructionEnd)
			{

				std::vector<AnalyzedInstruction>::iterator reference;

				// we are optimizing the search by doing this in a sorted list
				// will binary search be better?
				bool bReferenceInRange = false;
				if (it->referencedAddress > it->address)
				{
					reference = std::find(it, iterEnd, it->referencedAddress);
					bReferenceInRange = reference != iterEnd;
				}
				else
				{
					reference = std::find(iterBegin, it, it->referencedAddress);
					bReferenceInRange = reference != it;
				}

				// if the reference is in range of the analyzed data
				if (bReferenceInRange)
				{
					it->referencedInstruction = reference._Ptr;
					it->referencedIndex = reference - iterBegin;

					reference->referers.push_back(it._Ptr);

					it->fmtComment.push_back(0xffffffff, "j %llX", it->referencedAddress);
					reference->fmtComment.append_space(xRefString);
					reference->fmtComment += it->fmtAddress;

				}
			}

			if (it->isRefPointer)
			{
				UNUSED(m_memory->Read(it->referencedAddress, &it->referencedValue, pointerSize));
			}
		}	
	}
}

ImGui::TokenizedText FormatSymbolAddress(
	POINTER address,
	ResultGetSymbol& resultGetSymbol,
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
			symbol.push_back(pSymbol->fullName, settings.Symbol);
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
			symbol.push_back(pSymbol->fullName, settings.Symbol);

			if (isAtBaseModule) *isAtBaseModule = false;
			if (isAtSubroutineStart) *isAtSubroutineStart = true;
			return symbol;
		}
	}
}

void TokioAnalyzer::AnalyzeComment(AnalyzedData& data) noexcept
{
	static const auto& settings = Settings::theme.disasm;

	const size_t pointerSize = m_target->is32Bit() ? sizeof(dword_t) : sizeof(qword_t);

	for (AnalyzedInstruction& instruction : data.instructions)
	{
		if (!instruction.isRefPointer) continue;

		// a pointer reference
		if (instruction.referencedValue != 0)
		{
			auto resultGetModuleSymbol = m_symbol->AddressToModuleSymbol(instruction.referencedValue);

			// format as a symbol
			if (resultGetModuleSymbol.has_value())
			{
				instruction.fmtComment.append_space(FormatSymbolAddress(instruction.referencedValue, resultGetModuleSymbol));
				continue;
			}
		}

		// if it doesn't have a symbol, try to read it as a string
		static char stringBuffer[64];
		size_t byteRead = m_memory->Read(instruction.referencedAddress, stringBuffer, sizeof(stringBuffer));

		if (byteRead == sizeof(stringBuffer))
		{
			// hardcoded 5 valid chars to be defined as a string
			stringBuffer[sizeof(stringBuffer) - 1] = 0;


			// TODO: handle strings more nicely
			bool isValidString = strnlen_s(stringBuffer, 64) >= 5;
			bool isValidWString = false;
			if (!isValidString) isValidWString = wcsnlen_s(reinterpret_cast<wchar_t*>(stringBuffer), (sizeof(stringBuffer) - 1) / 2) >= 5;

			// it is a string!
			if (isValidString)
			{
				std::string comment(stringBuffer, sizeof(stringBuffer));
				auto findLineBreak = comment.rfind('\n');

				// strip \n out of the comment
				if (findLineBreak != std::string::npos)
					comment = comment.substr(0, findLineBreak);

				instruction.fmtComment.append_space({ settings.String, "\"%s\"", comment.c_str() });
				continue;
			}
			else if (isValidWString)
			{

				std::wstring wideComment(reinterpret_cast<wchar_t*>(stringBuffer), sizeof(stringBuffer) / 2);
				auto findLineBreak = wideComment.rfind(L'\n');

				// strip \n out of the comment
				if (findLineBreak != std::wstring::npos)
					wideComment = wideComment.substr(0, findLineBreak);

				instruction.fmtComment.append_space({ settings.String, "L\"%s\"", Tokio::String(wideComment).c_str()});
				continue;
			}
		}

		// if it's not a string, then might it be a pointer?
		POINTER referencePointer = 0;
		size_t bytesRead = m_memory->Read(instruction.referencedValue, &referencePointer, pointerSize);

		// it is a pointer to something else
		if (bytesRead == pointerSize && referencePointer != 0)
		{
			instruction.fmtComment.append_space({ settings.AddressAbs, "[%llX]", referencePointer });
			continue;
		}

		// it it's not anything above, just format it as a decimal value
		instruction.fmtComment.append_space({ settings.Displacement, "%llX", instruction.referencedValue });
	}
}


void TokioAnalyzer::AnalyzeRegion(
	POINTER address,
	POINTER size,
	const std::vector<byte_t>& buffer,
	size_t bufferOffset,
	size_t& instructionIndex,
	AnalyzedData& data
) EXCEPT
{
	try
	{

		// disassemble the region first
		std::vector<DisasmInstruction> disasmInstructions = 
			m_disassembler->Disassemble(address, buffer.data() + bufferOffset, size);

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
			dword_t mnemonicColor = Settings::GetDisasmColor(disasmData.mnemonic.type);

			// get the symbol data from this instruction address
			instruction.symbolAddress = m_symbol->AddressSymbolWalkNext(walkContext, disasmData.address);

			// copy the result from disasmData
			instruction.address           = disasmData.address;
			instruction.length            = disasmData.length;

			instruction.mnemonic          = std::move(disasmData.mnemonic);

			instruction.bufferOffset      = insBufferOffset;

			instruction.referencedAddress = disasmData.referencedAddress;
			instruction.isRefPointer      = disasmData.isRefPointer;

			instruction.operands          = std::move(disasmData.operands);
			instruction.isNotReadable = false;

			// format the address and mnemonic
			instruction.fmtMnemonic.push_back(disasmData.mnemonic.value, mnemonicColor);

			instruction.fmtAddress = std::move(FormatSymbolAddress(
				disasmData.address,
				instruction.symbolAddress,
				&instruction.isAtBaseModule,
				&instruction.isAtSubroutineStart
			));

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
					instruction.symbolReferenced = m_symbol->AddressToModuleSymbol(instruction.referencedAddress);
					instruction.fmtOperand += FormatSymbolAddress(instruction.referencedAddress, instruction.symbolReferenced);
				}
				// it's just a regular operand
				else
				{
					dword_t operandColor = Settings::GetDisasmColor(operand.type);
					instruction.fmtOperand.push_back(operand.value, operandColor);
				}
			}

			insBufferOffset += instruction.length;
		}

	}
	catch (Tokio::Exception& e)
	{
		e += "TokioAnalyzer failed to analyze region";
		throw e;
	}
}

void TokioAnalyzer::AnalyzeRegionNoSymbol(
	POINTER address,
	POINTER size,
	const std::vector<byte_t>& buffer,
	size_t bufferOffset,
	size_t& instructionIndex,
	AnalyzedData& data
) EXCEPT
{
	try
	{
		// disassemble the region first
		std::vector<DisasmInstruction> disasmInstructions =
			m_disassembler->Disassemble(address, buffer.data() + bufferOffset, size);

		size_t insBufferOffset = bufferOffset;


		// loop through all the disassembled instructions
		for (DisasmInstruction& disasmData : disasmInstructions)
		{
			AnalyzedInstruction& instruction = data.instructions[instructionIndex++];

			// color of the mnemonic base on its type
			dword_t mnemonicColor = Settings::GetDisasmColor(disasmData.mnemonic.type);


			// copy the result from disasmData
			instruction.address           = disasmData.address;
			instruction.length            = disasmData.length;

			instruction.mnemonic          = std::move(disasmData.mnemonic);

			instruction.bufferOffset      = insBufferOffset;

			instruction.referencedAddress = disasmData.referencedAddress;
			instruction.isRefPointer      = disasmData.isRefPointer;

			instruction.operands          = std::move(disasmData.operands);
			instruction.isNotReadable = false;

			// format the mnemonic, the address won't be formatted
			instruction.fmtMnemonic.push_back(disasmData.mnemonic.value, mnemonicColor);

			// here we parse the tokenized operands into colored text
			for (auto& operand : instruction.operands)
			{
				// skip the mnemonic and invalid operands (the mnemonic is already at the instruction.mnemonic)
				if (IsOperandMnemonic(operand.type) || operand.type == DisasmOperandType::Invalid) continue;

				if (operand.type == DisasmOperandType::WhiteSpace)
				{
					instruction.fmtOperand.push_back(" "s, 0x00000000);
				}
				// it's just a regular operand
				else
				{
					dword_t operandColor = Settings::GetDisasmColor(operand.type);
					instruction.fmtOperand.push_back(operand.value, operandColor);
				}
			}

			insBufferOffset += instruction.length;
		}

	}
	catch (Tokio::Exception& e)
	{
		e += "TokioAnalyzer failed to analyze region";
		throw e;
	}
}


_NODISCARD void TokioAnalyzer::Analyze(
	POINTER address,
	size_t size,
	AnalyzerFlags flags,
	std::vector<byte_t>& outBuffer,
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

		return;
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
			{
				AnalyzedInstruction& ins = outData.instructions[instructionIndex++];
				ins.address = offset;
				ins.isNotReadable = true;
			}
		}
		// disassemble valid region
		else
		{
			if (flags & AnalyzerFlags_::Symbol)
			{
				AnalyzeRegion(regionIter->start, regionIter->size, outBuffer, regionIter->start - address, instructionIndex, outData);
			}
			else
			{
				AnalyzeRegionNoSymbol(regionIter->start, regionIter->size, outBuffer, regionIter->start - address, instructionIndex, outData);
			}

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

	if (flags & AnalyzerFlags_::Subroutine)
	{
		AnalyzeCrossReferences(outData, instructionIndex);
		AnalyzeSubroutines(outData, instructionIndex, outBuffer);
	}
	else if (flags & AnalyzerFlags_::CrossReference)
	{
		AnalyzeCrossReferences(outData, instructionIndex);
	}

	if (flags & AnalyzerFlags_::Comment)
	{
		AnalyzeComment(outData);
	}

}

void TokioAnalyzer::AnalyzeRegionBasic(
	POINTER address,
	POINTER size,
	const std::vector<byte_t>& buffer,
	size_t& instructionIndex,
	AnalyzedData& data
) EXCEPT
{

	// disassemble the region first
	std::vector<DisasmInstruction> disasmInstructions =
		m_disassembler->Disassemble(address, buffer.data(), size);

	size_t insBufferOffset = 0;

	// loop through all the disassembled instructions
	for (DisasmInstruction& disasmData : disasmInstructions)
	{
		AnalyzedInstruction& instruction = data.instructions[instructionIndex++];

		// copy the result from disasmData
		instruction.address = disasmData.address;
		instruction.length = disasmData.length;

		instruction.mnemonic = std::move(disasmData.mnemonic);

		instruction.bufferOffset = insBufferOffset;

		instruction.referencedAddress = disasmData.referencedAddress;
		instruction.isRefPointer = disasmData.isRefPointer;

		instruction.operands = std::move(disasmData.operands);
		instruction.isNotReadable = false;


		instruction.blockIndex = UPTR_UNDEFINED;
		instruction.iSubroutine = UPTR_UNDEFINED;

		insBufferOffset += instruction.length;
	}
}

void TokioAnalyzer::FullAnalyze(FullAnalyzeResultCallback callback, const void* UserData) noexcept
{
	UNUSED(callback);
	UNUSED(UserData);


	FullAnalyedData result;
	size_t bufferSize = 1024'00;

	// invalidate all of the instructions
	AnalyzedData tempData;
	AnalyzedInstruction invalidInstruction(tempData);

	invalidInstruction.isNotReadable = false;
	tempData.instructions.resize(bufferSize, invalidInstruction);
	
	POINTER address = 0;
	VirtualMemoryInfo memInfo;

	std::vector<byte_t> memBuffer;
	memBuffer.resize(bufferSize);

	std::vector<MemoryReadRegion> regions;

	while (true)
	{
		try
		{
			memInfo = m_memory->VirtualQuery(address);
			address = memInfo.base + memInfo.size;

			if (memInfo.protect & VirtualProtection::Read && memInfo.protect & VirtualProtection::Execute)
			{
				MemoryReadRegion& region = regions.emplace_back();
				region.start = memInfo.base;
				region.size = memInfo.size;
			}
		}
		catch (Tokio::Exception)
		{
			break;
		}
	}


	for (MemoryReadRegion& region : regions)
	{
		POINTER endAddr = region.start + region.size;

		for (POINTER offset = region.start; offset < endAddr;)
		{
			size_t readSize = (endAddr - offset > bufferSize) ? bufferSize : endAddr - offset;
			size_t bytesRead = m_memory->Read(offset, memBuffer.data(), readSize);

			if (bytesRead == 0)
			{
				offset += bufferSize;
				continue;
			}

			size_t instructionIndex = 0;

			tempData.subroutines.clear();

			AnalyzeRegionBasic(offset, bytesRead, memBuffer, instructionIndex, tempData);
			//AnalyzeCrossReferences(tempData, instructionIndex);
			//AnalyzeSubroutines(tempData, instructionIndex, memBuffer);

			//if (tempData.subroutines.size() > 0)
			//{
			//	auto& sub = tempData.subroutines.back();
			//	auto& lastInstruction = tempData.instructions[sub.instructionIndex + sub.instructionCount - 1];
			//	offset = lastInstruction.address + lastInstruction.length;

			//	for (auto& subroutine : tempData.subroutines)
			//	{
			//		auto& copy = result.subroutines.emplace_back();
			//		copy.address = subroutine.address;
			//		copy.size = subroutine.size;

			//		printf("sub_%llx\n", copy.address);
			//	}
			//}
			//else
			//{
				auto& lastInstruction = tempData.instructions[instructionIndex - 1];
				offset = lastInstruction.address + lastInstruction.length;
			//}

					printf("sub_%llx\n", offset);
		}
	}

}


}
