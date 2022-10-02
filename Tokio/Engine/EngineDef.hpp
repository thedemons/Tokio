#pragma once

typedef unsigned __int64 POINTER;

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

struct ProcessModule
{
	// virtual base address of the module
	POINTER base = 0ull;

	// virtual entry point of the module
	POINTER entryPoint = 0ull;

	// size of the module
	size_t size = 0ull;

	std::string moduleNameA;
	std::string modulePathA;
	std::wstring moduleNameW;
	std::wstring modulePathW;

	// module exports
	std::vector<ModuleSymbol> exports;

	inline ModuleSymbol& AddExportSymbol()
	{
		return exports.emplace_back(this);
	};
};

// Contains all information about the target process
struct ProcessData
{
	// process id
	DWORD pid{0}; 

	// process handle for memory operation
	HANDLE handle{0};

	// all loaded modules
	std::vector<ProcessModule> modules;

	// is the process x86 or x64
	// TODO: Make an architecture enum
	BOOL is32bit = false;

	// the base image module
	ProcessModule* baseModule = nullptr;
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

	inline bool has_value() const
	{
		return pModule != nullptr;
	}

	inline bool has_symbol() const
	{
		return pSymbol != nullptr;
	}

	inline const ProcessModule* Module() const
	{
		return pModule;
	}

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
};

_NODISCARD _CONSTEXPR20 bool IsOperandMnemonic(DisasmOperandType type)
{
	return (type == DisasmOperandType::Mnemonic) ||
		static_cast<UINT>(type) >= static_cast<UINT>(DisasmOperandType::mneCall);
}


// Tokenized data
// Example:
//    type  = DisasmTokenType::Register
//    value = "RSP"
struct DisasmOperand
{
	char value[64]		 = {0}; // 64 bytes should be enough!
	DisasmOperandType type = DisasmOperandType::Invalid;

	// this operand is a reference to another address
	POINTER refAddress = 0ull;
};


struct DisasmInstruction
{
	// length of the instruction, in bytes
	size_t length = 0ull;

	// raw bytes of the instruction
	BYTE bytes[24]{0};

	// the absolute virtual address
	POINTER address = 0ull;

	// the instruction has a reference to another address
	POINTER refAddress = 0ull;

	// the reference address is a pointer, e.g: mov rax, [refAddress]
	bool isRefPointer = false;

	// the main operation of the instruction (Mnemonic)
	DisasmOperand mnemonic;

	// tokenized operands
	std::vector<DisasmOperand> operands;
};
