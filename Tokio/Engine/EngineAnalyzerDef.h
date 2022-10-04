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

struct AnalyzedData;
struct SubroutineInfo;

// block of codes belongs to the subroutine
struct SubroutineBlock
{
	// pointer to the next block, might be nullptr
	const SubroutineBlock* pNextBlock = nullptr;

	// pointer to the conditional block, might be mullptr
	const SubroutineBlock* pConditionalBlock = nullptr;

	// the start index of its instructions in the root
	// we don't want to each individual block to store its instructions
	// while we could query from the root AnalyzeData
	size_t insStartIndex = 0;

	// the end index of its instructions in the root
	size_t insEndIndex = 0;

	// reference to the root AnalyzeData of this block
	const AnalyzedData& root;

	// reference to the parent SubroutineInfo of this block
	const SubroutineInfo& subroutine;

	SubroutineBlock(const AnalyzedData& root, const SubroutineInfo& subroutine) :
		root(root), subroutine(subroutine)
	{
	}
};


struct SubroutineInfo
{
	// the start address of the subroutine
	POINTER start = 0;

	// the end address of the subroutine
	POINTER end = 0;

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
};



#endif // !TOKIO_ENGINE_ENGINE_ANALYZER_DEF_H
