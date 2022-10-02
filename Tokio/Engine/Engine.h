#ifndef TOKIO_ENGINE_H
#define TOKIO_ENGINE_H

#include "EngineDef.hpp"
#include "Memory/BaseMemory.hpp"
#include "Symbol/BaseSymbol.hpp"
#include "Disassembler/BaseDisassembler.hpp"


namespace Engine
{
// WARNING: These global variable is here only for
// the purpose of inlining functions, don't modify
// any of these directly, make a copy of the std::shared_ptr
// by invoking the method Target(), Memory() and Disassembler()

// TARGET PROCESS
inline std::shared_ptr<ProcessData> g_Target = nullptr;

// MEMORY ENGINE
inline std::shared_ptr<BaseMemory> g_Memory = nullptr;

// SYMBOL ENGINE
inline std::shared_ptr<BaseSymbol> g_Symbol = nullptr;

// DISASSEMBLE ENGINE
inline std::shared_ptr<BaseDisassembler> g_Disassembler = nullptr;


// callback when we attach to a process
typedef void (*LPON_ATTACH_CALLBACK)(std::shared_ptr<ProcessData>);

// Attach to a process
_NODISCARD auto Attach(DWORD pid) -> SafeResult(std::shared_ptr<ProcessData>);

// Detach target process
void Detach();

// Is attached to any process
bool IsAttached();

// Return a shared pointer of the target process
_NODISCARD _CONSTEXPR20 std::shared_ptr<ProcessData> Target()
{
	return g_Target;
}

// return a shared pointer to the memory engine
_NODISCARD _CONSTEXPR20 std::shared_ptr<BaseMemory> Memory()
{
	return g_Memory;
}

// return a shared pointer to the disassembler engine
_NODISCARD _CONSTEXPR20 std::shared_ptr<BaseDisassembler> Disassembler()
{
	return g_Disassembler;
}

// return a shared pointer to the disassembler engine
_NODISCARD _CONSTEXPR20 std::shared_ptr<BaseSymbol> Symbol()
{
	return g_Symbol;
}

_NODISCARD auto ReadMem(POINTER src, void* dest, size_t size)->SafeResult(void);
_NODISCARD auto WriteMem(POINTER dest, const void* src, size_t size)->SafeResult(void);

template <typename Type>
_NODISCARD _CONSTEXPR20 auto ReadMem(POINTER address)->SafeResult(Type) { return Memory()->Read<Type>(address); }

template <typename Type>
_NODISCARD _CONSTEXPR20 auto WriteMem(POINTER address, Type value)->SafeResult(void) { return Memory()->Write<Type>(address, value); }


void OnAttachCallback(LPON_ATTACH_CALLBACK callback);

}

#endif // !TOKIO_ENGINE_H