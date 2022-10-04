#pragma once
#ifndef TOKIO_ENGINE_ENGINEDEF_H
#define TOKIO_ENGINE_ENGINEDEF_H
#include <string>
#include <vector>
#include <Windows.h>

// We want to use an unsigned __int64 for the virtual remote address
typedef unsigned __int64 POINTER;

// ================================== MEMORY DEFINITIONS ==================================
 
// used for ReadMemSafe
struct MemoryReadRegion
{
	// Start address of the region
	// 
	// NOTICE: This is not the starting address of the region (page)
	//		   but the starting address of the read region
	// 
	// Example: ReadMemSafe(0x100, 0x200) // read from 0x100 to 0x200
	//          Where the readable region is from 0x80 to 0x140
	//          The start address will be 0x100, and the size will be 0x40 (0x140 - 0x100)
	POINTER start = 0;

	// size of the read region
	size_t size = 0;

	// quick function to calculate the end address
	inline size_t end() const noexcept { return start + size; }
};


// FIXME: This enum is ugly, FIXITPLEASE
typedef int VirtualProtection_;
class VirtualProtection
{
public:
enum Protection
{
	NoAccess     = 0,		// No access at all, the region is not readable
	Guard        = 1 << 0,  // Guarded region
	Read         = 1 << 1,  // Readable region, it should always be Read | OtherFlags, except for NoAccess
	Write        = 1 << 2,  // Writable region
	WriteCopy    = 1 << 2,  // Write on copy region
	Execute      = 1 << 4,  // Executable region
};
};

// used for BaseMemory::VirtualQuery()
struct VirtualMemoryInfo
{
	// base address of the region
	POINTER base;
	
	// size of the region
	size_t size;

	// the region's protection
	VirtualProtection_ protect;
};


// ================================== SYMBOL DEFINITIONS ==================================

struct ProcessModule;

struct ModuleSymbol
{
	UINT ordinal   {0}   ;		// ordinal index
	POINTER offset {0}   ;	    // offset from the base
	size_t size	   {0}   ;		// size of the subroutine
	std::string name     ;	    // symbol name
	ProcessModule* parent;	    // pointer to the module of this symbol

	ModuleSymbol(ProcessModule* parent) : parent(parent){}
};


// The process module, contains basic information such as 
// the module file path and name, base address
struct ProcessModule
{
	// virtual base address of the module
	POINTER base = 0ull;

	// virtual entry point of the module
	// we are not using this for now
	POINTER entryPoint = 0ull;

	// size of the module
	size_t size = 0ull;

	std::string moduleNameA;
	std::string modulePathA;

	std::wstring moduleNameW;
	std::wstring modulePathW;

	// all of the exports of this module
	// this vector remains empty until BaseSymbol::Update has been called
	std::vector<ModuleSymbol> exports;

	// warning: only BaseSymbol and its derived engines should be using this
	inline ModuleSymbol& AddExportSymbol()
	{
		return exports.emplace_back(this);
	};
};


// LEGACY
// variant used for symbol linking between symbol data and disassembled instruction
// using VariantModuleSymbol = std::variant<ProcessModule, ModuleSymbol>;

// use for symbol lookup
// we use this instead of VariantModuleSymbol for a better performance
// symbol lookup is a really frequent operation
class ResultGetSymbol
{
private:
	const ProcessModule* pModule;
	const ModuleSymbol* pSymbol;

public:
	ResultGetSymbol(const ProcessModule* pModule, const ModuleSymbol* pSymbol) :
		pModule(pModule), pSymbol(pSymbol)
	{

	}

	ResultGetSymbol(const ProcessModule* pModule) :
		pModule(pModule), pSymbol(nullptr)
	{

	}

	ResultGetSymbol() :
		pModule(nullptr), pSymbol(nullptr)
	{

	}

	// has_value() will return false if the lookup failed, the address has no symbol
	inline bool has_value() const
	{
		return pModule != nullptr;
	}

	// has_symbol() will return true if the address has both module and symbol
	// if (has_value() && !has_symbol()) then the address only has a linked module
	inline bool has_symbol() const
	{
		return pSymbol != nullptr;
	}

	// pointer to the module of this address
	inline const ProcessModule* Module() const
	{
		return pModule;
	}

	// pointer to the symbol of this address
	inline const ModuleSymbol* Symbol() const
	{
		return pSymbol;
	}
};


// Used for BaseSymbol::AddressSymbolWalk()
class SymbolWalkContext
{
private:
	// copy of the modules, maybe change this to a reference to improve performance?
	// these modules and its symbols must be sorted in order for this to work!
	const std::vector<ProcessModule>& modules;

	size_t module_index = 0ull;
	size_t symbol_index = UPTR_UNDEFINED;

public:
	SymbolWalkContext(const std::vector<ProcessModule>& modules) : modules(modules) {}

	// get the current module
	_NODISCARD _CONSTEXPR20 const ProcessModule* Module() const noexcept
	{
		return &modules[module_index];
	}

	// get the current symbol
	_NODISCARD _CONSTEXPR20 const ModuleSymbol* Symbol() const noexcept
	{
		if (symbol_index == UPTR_UNDEFINED) return nullptr;
		return &modules[module_index].exports[symbol_index];
	}

	// check if the address belongds to any of the symbols
	_NODISCARD _CONSTEXPR20 bool IsAddressOfSymbols(POINTER address) const noexcept
	{
		auto& procMod = modules[module_index];

		if (procMod.exports.size() == 0) return false;
		
		POINTER RvaOffset = address - procMod.base;
		auto& firstSymbol = procMod.exports.front();
		auto& lastSymbol = procMod.exports.back();

		return firstSymbol.offset <= RvaOffset && RvaOffset <= lastSymbol.offset + lastSymbol.size;
	}

	// increase the module index by one, if out of bound then set to zero and return false, which means the walk ended
	// module_index is set to zero to avoid invalid access by Module(), you must handle the return value of this function
	_NODISCARD _CONSTEXPR20 bool NextModule() noexcept
	{
		if (++module_index >= modules.size())
		{
			module_index = 0;
			return false;
		}

		return true;
	}

	// increase the symbol index by one, if out of bound then set to UPTR_UNDEFINED and return false
	_NODISCARD _CONSTEXPR20 bool NextSymbol() noexcept
	{
		if (++symbol_index >= modules[module_index].exports.size())
		{
			symbol_index = UPTR_UNDEFINED;
			return false;
		}

		return true;
	}
};

// ================================== DISASSEMBLER DEFINITIONS ==================================

// The type of the operand
enum class DisasmOperandType : UINT
{
	Invalid,          // An invalid token
	WhiteSpace,       // A whitespace character.
	Delimeter,        // A delimiter character (like `,`, `:`, `+`, `-`, `*`).
	ParenthesisOpen,  // An open parenthesis character (like '(', `[`, `{`).
	ParenthesisClose, // A close parenthesis character (like `)`, `]`, `}`).
	Prefix,           // A prefix literal (like "LOCK", "REP").
	Mnemonic,         // A mnemonic literal (like "MOV", "VCMPPSD", "LCALL").
	Register,         // A register literal (like "RAX", "DS", "%ECX").
	AddressAbs,       // An absolute address literal (like `0x00400000`).
	AddressRel,       // A relative address literal (like `-0x100`).
	Displacement,     // A displacement literal (like `0xFFFFFFFF`, `-0x100`, `+0x1234`).
	Immediate,        // An immediate literal (like `0xC0`, `-0x1234`, `$0x0000`).
	TypeCast,         // A typecast literal (like `DWORD PTR`).
	Decorator,        // A decorator literal (like "Z", "1TO4").
	Literal,          // A symbol literal

	// extra mnemonic types
	mneCall,			// call instruction
	mneSyscall,			// syscall instruction
	mneJump,			// JMP directly
	mneJumpCondition,	// jump with conditions (je, jne, jz,...)
	mneReturn,			// ret instruction
	mneInt3,			// int3 break instruction
};

// Whether the operand is mnemonic
_NODISCARD _CONSTEXPR20 bool IsOperandMnemonic(DisasmOperandType type)
{
	return (type == DisasmOperandType::Mnemonic) ||
		static_cast<UINT>(type) >= static_cast<UINT>(DisasmOperandType::mneCall);
}


// Operands belong to an tokenized instruction
// 
// An instruction may have multiple operands
// For example:
//	  opcode:		  mov rax, rsp
//    operands value: { "mov", " ", "rax", ",", " ", "rsp" }
//	  operands type:  { Mnemonic, WhiteSpace, Register, Delimeter, WhiteSpace, Register}
struct DisasmOperand
{
	char value[64]		 = {0}; // 64 bytes should be enough!
	DisasmOperandType type = DisasmOperandType::Invalid;

	// this operand is a reference to another address
	POINTER refAddress = 0ull;
};


// Output of the BaseDisassembler
//
// Contains a summary of information on the instruction, including tokenized operands
struct DisasmInstruction
{
	// length of the instruction, in bytes
	size_t length = 0ull;

	// the absolute virtual address
	POINTER address = 0ull;

	// the instruction has a reference to another address
	// example: jne 0x7FFA1E370
	// the refAddress here is 0x7FFA1E370
	POINTER refAddress = 0ull;

	// the reference address is a pointer e.g: mov rax, [refAddress]
	bool isRefPointer = false;

	// the main operation of the instruction (Mnemonic)
	DisasmOperand mnemonic;

	// tokenized operands
	std::vector<DisasmOperand> operands;
};

// ================================== ANALYZER DEFINITIONS ==================================

struct AnalyzeData;
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
	const AnalyzeData& root;

	// reference to the parent SubroutineInfo of this block
	const SubroutineInfo& subroutine;

	SubroutineBlock(const AnalyzeData& root, const SubroutineInfo& subroutine) : 
		root(root), subroutine(subroutine) {}
};


struct SubroutineInfo
{
	// the start address of the subroutine
	POINTER start = 0;

	// the end address of the subroutine
	POINTER end = 0;

	// reference to the root AnalyzeData of this subroutine
	const AnalyzeData& root;

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
	//		0x0B	je		0x2A     >--------------;
	//												|
	//	 block_0D:                                  |
	//		0x0D	lea		eax, [rdx-01h]          |
	//		0x10	test	edx, eax                |
	//		0x12	jnz		0x2A	>-----------;	|
	//										    |	|
	//	 block_14:								|	|
	//		0x14	imul	ecx, edx, -02		|	|
	//		0x17	and		ecx, [rbx]			|	|
	//		0x19	call	0x30C				|	|
	//		0x1E	mov		eax, eax			|	|
	//		0x20	add		rax, rbx			|	|
	//											|	|
	//	 block_23:  <<-------------------;      |	|
	//		0x23	add		rsp, 20h	 |	    |	|
	//		0x27	pop		rbx			 |	    |	|
	//		0x28	ret					 |		|	|
	//		0x29	int3				 |	    |	|
	//									 |	    |	|
	//	 block_2A:  <<===================+======+---'
	//		0x2A	xor		eax, eax	 |
	//		0x2C	jmp		0x23	>----'
	//		0x2E	int3
	// 
	//	 sub_0  endp
	//
	// The subroutine above start at address 0x00 and end at 0x2E
	// with 5 blocks in total

	SubroutineInfo(const AnalyzeData& root) : root(root) {};
};

// Output of the BaseAnalyzer
struct AnalyzeData
{
	// a list of subroutines
	std::vector<SubroutineInfo> subroutines;

	// the memory buffer of the analyzed data
	std::vector<BYTE> memoryBuffer;

	// the analyzed instructions
	std::vector<DisasmInstruction> instructions;
};



// ================================== MAIN DEFINITIONS ==================================

// Contains information about the target process
struct ProcessData
{
	// process id
	DWORD pid{ 0 };

	// process handle for memory operation
	HANDLE handle{ 0 };

	// all loaded modules
	std::vector<ProcessModule> modules;

	// is the process x86 or x64
	// TODO: Make an architecture enum
	BOOL is32bit = false;

	// the base image module
	ProcessModule* baseModule = nullptr;
};


#endif // !TOKIO_ENGINE_ENGINEDEF_H