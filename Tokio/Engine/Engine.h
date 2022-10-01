#ifndef TOKIO_ENGINE_H
#define TOKIO_ENGINE_H

#include "EngineDef.hpp"
#include "Memory/BaseMemory.hpp"
#include "Disassembler/BaseDisassembler.hpp"


namespace Engine
{
// callback when we attach to a process
typedef void (*LPON_ATTACH_CALLBACK)(std::shared_ptr<ProcessData>);

// Attach to a process
_NODISCARD auto Attach(DWORD pid) -> SafeResult(std::shared_ptr<ProcessData>);

// Detach target process
void Detach();

// Is attached to any process
bool IsAttached();

// Return a shared pointer of the target process
_NODISCARD std::shared_ptr<ProcessData> Target();

// return a shared pointer to the memory engine
_NODISCARD std::shared_ptr<BaseMemory> Memory();

// return a shared pointer to the disassembler
_NODISCARD std::shared_ptr<BaseDisassembler> Disassembler();

_NODISCARD auto ReadMem(POINTER src, void* dest, size_t size)->SafeResult(void);
_NODISCARD auto WriteMem(POINTER dest, const void* src, size_t size)->SafeResult(void);

template <typename Type>
_NODISCARD _CONSTEXPR20 auto ReadMem(POINTER address)->SafeResult(Type) { return Memory()->Read<Type>(address); }

template <typename Type>
_NODISCARD _CONSTEXPR20 auto WriteMem(POINTER address, Type value)->SafeResult(void) { return Memory()->Write<Type>(address, value); }


void OnAttachCallback(LPON_ATTACH_CALLBACK callback);

}

#endif // !TOKIO_ENGINE_H