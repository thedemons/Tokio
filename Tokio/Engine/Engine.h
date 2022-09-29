#ifndef TOKIO_ENGINE_H
#define TOKIO_ENGINE_H

#include "EngineDef.hpp"
#include "Memory/BaseMemory.hpp"


namespace Engine
{

inline BaseMemory* g_MemoryEngine = nullptr;

inline DWORD g_pid = 0;
inline HANDLE g_handle = nullptr;

inline std::vector<ModuleData> g_moduleList;


void Init();

// MEMORY STUFF ============================================================================
auto Attach(DWORD pid) -> SafeResult(void);
_CONSTEXPR20 void Detach();

_CONSTEXPR20 DWORD GetPID();
_CONSTEXPR20 HANDLE GetHandle();

// return a pointer to the memory engine
constexpr BaseMemory* Memory();

_CONSTEXPR20 auto ReadMem(POINTER src, void* dest, size_t size)->SafeResult(void);
_CONSTEXPR20 auto WriteMem(POINTER dest, const void* src, size_t size)->SafeResult(void);

template <typename Type>
_CONSTEXPR20 auto ReadMem(POINTER address)->SafeResult(Type)
{
	return Memory()->Read<Type>(address);
}

template <typename Type>
_CONSTEXPR20 auto WriteMem(POINTER address, Type value)->SafeResult(void)
{
	return Memory()->Write<Type>(address, value);
}

_CONSTEXPR20 std::vector<ModuleData>& GetModuleList()
{
	return g_moduleList;
}
}

#endif // TOKIO_ENGINE_H