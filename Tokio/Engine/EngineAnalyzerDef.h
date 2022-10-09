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
#ifndef TOKIO_ENGINE_ENGINE_ANALYZER_DEF_H
#define TOKIO_ENGINE_ENGINE_ANALYZER_DEF_H

// NOTICE: This header was separated from EngineDef.h because it needs to include
//		   TokenzedText.h, which includes imgui.h, and doing that in EngineDef.h will
//		   leak imgui's declarations to the engine, that is nothing too serious but I
//		   find it ugly, so it was separated

#include "EngineDef.h"
#include "GUI/ImGui/custom/TokenizedText.h"

// ================================== ANALYZER DEFINITIONS ==================================

struct AnalyzedInstruction;
struct AnalyzedData;
struct SubroutineBlock;
struct SubroutineInfo;

struct SubroutineInfo
{
	// the start address of the subroutine
	POINTER address = 0;

	// the end address of the subroutine
	POINTER size = 0;

	size_t instructionIndex = UPTR_UNDEFINED;
	size_t instructionCount = 0;

	// reference to the root AnalyzeData of this subroutine
	const AnalyzedData& root;

	// a list of blocks of the subroutine
	// blocks are separated by jmp and conditional jumps
	std::vector<SubroutineBlock> blocks;

	// Example:
	// 
	//	sub_0  start
	// 
	//		0x00	push rbx
	//		0x02	sub		rsp, 20h
	//		0x06	mov		rbx, rcx
	//		0x09	test	[rcx], edx
	//		0x0B	je		0x2A	>-----------;
	//											|
	//	 block_0D:								|
	//		0x0D	lea		eax, [rdx-01h]		|
	//		0x10	test	edx, eax			|
	//		0x12	jnz		0x2A	>-------;	|
	//										|	|
	//	 block_14:							|	|
	//		0x14	imul	ecx, edx, -02	|	|
	//		0x17	and		ecx, [rbx]		|	|
	//		0x19	call	0x30C			|	|
	//		0x1E	mov		eax, eax		|	|
	//		0x20	add		rax, rbx		|	|
	//										|	|
	//	 block_23:	<<------------------;	|	|
	//		0x23	add		rsp, 20h	|	|	|
	//		0x27	pop		rbx			|	|	|
	//		0x28	ret					|	|	|
	//		0x29	int3				|	|	|
	//									|	|	|
	//	 block_2A:  <<==================+===+---'
	//		0x2A	xor		eax, eax	|
	//		0x2C	jmp		0x23	>---'
	//		0x2E	int3
	// 
	//	 sub_0  endp
	//
	// The subroutine above start at address 0x00 and end at 0x2E
	// with 5 blocks in total

	SubroutineInfo(const AnalyzedData& root) : root(root) {};

	_NODISCARD _CONSTEXPR20 SubroutineBlock& AddBlock()
	{
		return blocks.emplace_back(root, *this);
	}

	// get a block at x index
	_NODISCARD _CONSTEXPR20  const SubroutineBlock& GetBlock(size_t index) const
	{
		return blocks[index];
	}
};


// block of codes belongs to the subroutine
struct SubroutineBlock
{
	// the reason we don't use pointers here is because when analyzing
	// we didn't know the maxium size of blocks a subroutine can have
	// and therefore couldn't use resize() and must use emplace_back()
	// that reallocate the buffer everytime making the pointer obsolete
	// use the GetNextBlock() and GetConditionalBlock() methods for easier access
	
	// index of the next block in SubroutineInfo
	size_t nextBlockIndex = UPTR_UNDEFINED;

	// index of the next conditional block in SubroutineInfo
	size_t nextCondBlockIndex = UPTR_UNDEFINED;

	// index of the previous block in SubroutineInfo
	size_t prevBlockIndex = UPTR_UNDEFINED;

	// index of the previous conditional block in SubroutineInfo
	size_t prevCondBlockIndex = UPTR_UNDEFINED;

	// the start index of its instructions in the root
	// we don't want each individual block to store its instructions
	// while we could query from the root AnalyzedData
	size_t instructionIndex = UPTR_UNDEFINED;

	// number of instructions
	size_t instructionCount = 0;

	// reference to the root AnalyzeData of this block
	const AnalyzedData& root;

	// reference to the parent SubroutineInfo of this block
	const SubroutineInfo& subroutine;

	SubroutineBlock(const AnalyzedData& root, const SubroutineInfo& subroutine) :
		root(root), subroutine(subroutine)
	{
	}

	// Get the next block
	_NODISCARD _CONSTEXPR20 const SubroutineBlock* GetNextBlock() const
	{
		return (nextBlockIndex >= 0 && nextBlockIndex < subroutine.blocks.size()) ? &subroutine.blocks[nextBlockIndex] : nullptr;
	}

	// Get the next conditional block
	_NODISCARD _CONSTEXPR20 const SubroutineBlock* GetNextCondBlock() const
	{
		return (nextCondBlockIndex >= 0 && nextCondBlockIndex < subroutine.blocks.size()) ? &subroutine.blocks[nextCondBlockIndex] : nullptr;
	}

	// Get the previous block
	_NODISCARD _CONSTEXPR20 const SubroutineBlock* GetPrevBlock() const
	{
		return (prevBlockIndex >= 0 && prevBlockIndex < subroutine.blocks.size()) ? &subroutine.blocks[prevBlockIndex] : nullptr;
	}

	// Get the previous conditional block
	_NODISCARD _CONSTEXPR20 const SubroutineBlock* GetPrevCondBlock() const
	{
		return (prevCondBlockIndex >= 0 && prevCondBlockIndex < subroutine.blocks.size()) ? &subroutine.blocks[prevCondBlockIndex] : nullptr;
	}

};



// Output of the BaseAnalyzer
//
//  ## WARNING-PERFORMANCE WARNING-FIXME: (THIS IS NOT ENABLED FOR NOW)
//	We have a seperated vector of instructions and its extra data because
//	we used std::move to move the result returned from BaseDisassembler::Disasm
//	into this struct, while we could copy its content into AnalyzedInstruction
//	We basically sacrifice code-readability for a little boost of performance.
struct AnalyzedData
{
	// a list of subroutines
	std::vector<SubroutineInfo> subroutines;

	// the analyzed instructions
	std::vector<AnalyzedInstruction> instructions;

	// the analyzed instructions
	//std::vector<AnalyzedInstruction> analyzedInstructions;

	_NODISCARD _CONSTEXPR20 SubroutineInfo& AddSubroutine()
	{
		return subroutines.emplace_back(*this);
	}

	_NODISCARD _CONSTEXPR20 AnalyzedInstruction& AddInstruction()
	{
		return instructions.emplace_back(*this);
	}

	AnalyzedData(){}

	AnalyzedData(const AnalyzedData& o)
	{
		printf("i was copied %lld\n", o.instructions.size());

	}

	// return an instruction at x index
	_NODISCARD _CONSTEXPR20 const AnalyzedInstruction& GetInstruction(size_t index) const
	{
		return instructions[index];
	}

	// return a subroutine at x index
	_NODISCARD _CONSTEXPR20 const SubroutineInfo& GetSubroutine(size_t index) const
	{
		return subroutines[index];
	}

	//void copy(const AnalyzedData& o)
	//{
	//	// FIXME-PERFORMANCE
	//	// We could do this ONLY IF IT IS NOT PERFORMANCE-CRITICAL
	//	// We must copy and reconstruct one-by-one because it has a reference to AnalyzedData and other stuffs
	//	// 
	//	// i know i know.. it's ugly

	//	this->instructions.clear();
	//	this->subroutines.clear();
	//	this->instructions.reserve(o.instructions.size());
	//	this->subroutines.reserve(o.subroutines.size());

	//	for (const AnalyzedInstruction& ins : o.instructions)
	//	{
	//		auto& copiedIns = this->AddInstruction();
	//		copiedIns.address = ins.address;
	//		copiedIns.length = ins.length;
	//		copiedIns.bufferOffset = ins.bufferOffset;
	//		copiedIns.referencedAddress = ins.referencedAddress;
	//		copiedIns.referencedIndex = ins.referencedIndex;
	//		copiedIns.mnemonic = ins.mnemonic;
	//		copiedIns.operands = ins.operands;
	//		copiedIns.blockIndex = ins.blockIndex;
	//		copiedIns.referencedValue = ins.referencedValue;
	//		copiedIns.isRefPointer = ins.isRefPointer;
	//		copiedIns.isNotReadable = ins.isNotReadable;
	//		copiedIns.isAtBaseModule = ins.isAtBaseModule;
	//		copiedIns.isAtSubroutineStart = ins.isAtSubroutineStart;
	//		copiedIns.isAtSubroutineEnd = ins.isAtSubroutineEnd;
	//		copiedIns.fmtAddress = ins.fmtAddress;
	//		copiedIns.fmtComment = ins.fmtComment;
	//		copiedIns.fmtMnemonic = ins.fmtMnemonic;
	//		copiedIns.fmtOperand = ins.fmtOperand;


	//		// calculate the address before-hand here, no issue...
	//		// don't worry about std::vector reallocate since we reserved enough size
	//		for (auto& referer : ins.referers)
	//		{
	//			size_t index = referer - &o.instructions[0];
	//			copiedIns.referers.emplace_back(&instructions[0] + index);
	//		}

	//		size_t index = ins.referencedInstruction - &o.instructions[0];
	//		copiedIns.referencedInstruction = &instructions[0] + index;
	//	}

	//	this->subroutines.reserve(o.subroutines.size());

	//	for (const SubroutineInfo& sub : o.subroutines)
	//	{
	//		auto& copiedSub = this->AddSubroutine();
	//		copiedSub.start = sub.start;
	//		copiedSub.end = sub.end;
	//		copiedSub.blocks.reserve(sub.blocks.size());

	//		for (const SubroutineBlock& block : sub.blocks)
	//		{
	//			auto& copiedBlock = copiedSub.AddBlock();
	//			copiedBlock.nextBlockIndex = block.nextBlockIndex;
	//			copiedBlock.nextCondBlockIndex = block.nextCondBlockIndex;
	//			copiedBlock.insStartIndex = block.insStartIndex;
	//			copiedBlock.insEndIndex = block.insEndIndex;
	//		}
	//	}

	//	for (size_t n = 0; n < o.instructions.size(); n++)
	//	{
	//		assert(instructions[n].address == o.instructions[n].address);
	//	}
	//	for (size_t n = 0; n < o.subroutines.size(); n++)
	//	{
	//		assert(subroutines[n].start == o.subroutines[n].start);
	//		assert(subroutines[n].end == o.subroutines[n].end);
	//	}
	//}
};



// Analyzed data of the instruction, such as cross-references, symbols
// indexes and tokenized text for the user-interface
struct AnalyzedInstruction
{
	// a reference to the root AnalyzedData
	const AnalyzedData& root;

	// the absolute virtual address
	POINTER address = 0ull;

	// length of the instruction, in bytes
	size_t length = 0ull;

	// the offset of the instruction from the start of the memory buffer
	size_t bufferOffset = 0;

	// the instruction has a reference to another address
	// example: jne 0x7FFA1E370
	// the refAddress here is 0x7FFA1E370
	POINTER referencedAddress = 0ull;

	// the referenced index in the vector
	size_t referencedIndex = UPTR_UNDEFINED;

	// the main operation of the instruction (Mnemonic)
	DisasmOperand mnemonic;

	// tokenized operands
	std::vector<DisasmOperand> operands;

	// a list of pointer to the instructions that refered to this instructions
	// the pointer points to the object in the same vector, it should be alive
	// until the list is destroyed, becareful about memory management
	std::vector<AnalyzedInstruction*> referers;

	// a pointer to the referenced instruction
	AnalyzedInstruction* referencedInstruction = nullptr;

	// the symbol for the address of the instruction, if it has any
	ResultGetSymbol symbolAddress;

	// the symbol for the referenced address, if it has any
	ResultGetSymbol symbolReferenced;

	// the index of the block if it belongs to any blocks
	size_t blockIndex = UPTR_UNDEFINED;

	// the index of the subroutine if it belongs to any subroutines
	size_t iSubroutine = UPTR_UNDEFINED;

	// the value of referencedAddress if is is a pointer (isRefPointer)
	POINTER referencedValue = 0ull;

	// the referenced address is a pointer e.g: mov rax, [referencedAddress]
	bool isRefPointer = false;

	// true if the memory address was not readble
	bool isNotReadable = true;

	// true if the instruction is at the start of a module
	bool isAtBaseModule = false;

	// true if the instruction is at the starting point of a subroutine
	bool isAtSubroutineStart = false;

	// true if the instruction is at the end point of a subroutine
	bool isAtSubroutineEnd = false;

	// formatted address, includes symbol name
	// example: ntdll.NtProtectVirtualMemory+5
	ImGui::TokenizedText fmtAddress;

	// formatted comment, if the address has a reference/referer
	// this can includes: string, referenced address symbol, cross-ref,..
	ImGui::TokenizedText fmtComment;

	// formatted mnemonic
	ImGui::TokenizedText fmtMnemonic;

	// formatted operands, this does not include the mnemonic
	ImGui::TokenizedText fmtOperand;

	AnalyzedInstruction(const AnalyzedData& root) : root(root) {}

	// for searching the vector of analyzed instructions
	bool operator==(const AnalyzedInstruction& v)
	{
		return address == v.address;
	};
	bool operator==(const POINTER& v)
	{
		return address == v;
	};


	_NODISCARD _CONSTEXPR20 const SubroutineInfo* GetSubroutine() const
	{
		return (iSubroutine >= 0 && iSubroutine < root.subroutines.size()) ? &root.subroutines[iSubroutine] : nullptr;
	}
};


// contains only the address end size of the subroutine
struct SubroutineBasicInfo
{
	POINTER address = 0;
	POINTER size    = 0;
};

// Full analyzed data
struct FullAnalyedData
{
	std::map<POINTER, std::vector<POINTER>> xrefs;
	std::vector<SubroutineBasicInfo> subroutines;
};

#endif // !TOKIO_ENGINE_ENGINE_ANALYZER_DEF_H
