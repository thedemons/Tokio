#ifndef TOKIO_ENGINE_H
#define TOKIO_ENGINE_H

#include "EngineDef.h"

#include "Plugins/PluginsDef.h"

#include "Memory/BaseMemory.h"
#include "Symbol/BaseSymbol.h"
#include "Disassembler/BaseDisassembler.h"
#include "Decompiler/BaseDecompiler.h"
#include "Analyzer/BaseAnalyzer.h"

#include <memory>


namespace Engine
{

// WARNING: These global variables are here only for
// the purpose of inlining functions, don't modify
// any of these directly, make a copy of the std::shared_ptr
// by invoking the method Target(), Memory(), Symbol() or Disassembler()
// the reason for std::shared_ptr despite the fact that it's slow is
// for the sake of safe threading operations, we don't want do destroy
// the engines if any of the threads using them is still running

// TARGET PROCESS
inline std::shared_ptr<ProcessData>      g_Target       = nullptr;

// MEMORY ENGINE
inline std::shared_ptr<BaseMemory>       g_Memory       = nullptr;

// SYMBOL ENGINE
inline std::shared_ptr<BaseSymbol>       g_Symbol       = nullptr;

// DISASSEMBLE ENGINE
inline std::shared_ptr<BaseDisassembler> g_Disassembler = nullptr;

// DECOMPILE ENGINE
inline std::shared_ptr<BaseDecompiler>   g_Decompiler   = nullptr;

// ANALYZER ENGINE
inline std::shared_ptr<BaseAnalyzer>     g_Analyzer     = nullptr;

// LOADED PLUGIN INTERFACES
inline std::vector<PluginInterface>      g_Plugins;

// callback when we attach to a process
typedef void (*LPON_ATTACH_CALLBACK)(std::shared_ptr<ProcessData>);

// callback when detach the target process
typedef void (*LPON_DETACH_CALLBACK)();

void Init() noexcept;

// Attach to a process
void Attach(PID pid) EXCEPT;

// Detach target process
void Detach() noexcept;

// Is attached to any process
_NODISCARD _CONSTEXPR20 bool IsAttached() noexcept
{
	return g_Target != nullptr;
}

// Is the target process 32 bit
_NODISCARD _CONSTEXPR20 bool Is32Bit() noexcept
{
	//assert(g_Target != nullptr);
	return g_Target->is32Bit();
}

// Return a shared pointer of the target process
_NODISCARD _CONSTEXPR20 std::shared_ptr<ProcessData> Target() noexcept
{
	return g_Target;
}

// return a shared pointer to the memory engine
_NODISCARD _CONSTEXPR20 std::shared_ptr<BaseMemory> Memory() noexcept
{
	return g_Memory;
}

// return a shared pointer to the disassembler engine
_NODISCARD _CONSTEXPR20 std::shared_ptr<BaseDisassembler> Disassembler() noexcept
{
	return g_Disassembler;
}


// return a shared pointer to the disassembler engine
_NODISCARD _CONSTEXPR20 std::shared_ptr<BaseDecompiler> Decompiler() noexcept
{
	return g_Decompiler;
}

// return a shared pointer to the analyzer engine
_NODISCARD _CONSTEXPR20 std::shared_ptr<BaseAnalyzer> Analyzer() noexcept
{
	return g_Analyzer;
}

// return a shared pointer to the disassembler engine
_NODISCARD _CONSTEXPR20 std::shared_ptr<BaseSymbol> Symbol() noexcept
{
	return g_Symbol;
}

_NODISCARD _CONSTEXPR20 size_t ReadMem(POINTER src, void* dest, size_t size) noexcept
{
	//assert(g_Memory != nullptr);
	return g_Memory->Read(src, dest, size);
}

// Read memory of the region that may cross a no-read-access page
_CONSTEXPR20 void ReadMemSafe(
	POINTER address,
	byte_t* buffer,
	size_t size,
	std::vector<MemoryReadRegion>& regions
) noexcept
{
	return g_Memory->ReadMemSafe(address, buffer, size, regions);
}

_NODISCARD _CONSTEXPR20 size_t WriteMem(POINTER dest, const void* src, size_t size) noexcept
{
	return g_Memory->Write(dest, src, size);
}

template <typename Type>
_NODISCARD _CONSTEXPR20 Type ReadMem(POINTER address) EXCEPT
{
	return g_Memory->Read<Type>(address);
}

template <typename Type>
_NODISCARD _CONSTEXPR20 size_t WriteMem(POINTER address, const Type& value) noexcept
{
	return g_Memory->Write<Type>(address, value);
}

// query information of a virtual address
_NODISCARD _CONSTEXPR20 VirtualMemoryInfo VirtualQuery(POINTER address) EXCEPT
{
	return g_Memory->VirtualQuery(address);
}


// Analyze a memory region
// address:		      the virtual address in the target process
// size:		      the size in bytes to analyze
// bDisectSubroutine: true to disect subroutine, if false, the subroutines vector will have a size of zero, only the instructions are analyzed
// outBuffer:		  the output buffer contains read memory
// outData:			  the output analyzed data	
_NODISCARD _CONSTEXPR20 void Analyze(
	POINTER address,
	size_t size,
	AnalyzerFlags flags,
	std::vector<byte_t>& outBuffer,
	AnalyzedData& outData
) EXCEPT
{
	return g_Analyzer->Analyze(address, size, flags, outBuffer, outData);
}


_NODISCARD _CONSTEXPR20 std::string Decompile(POINTER address, size_t size) EXCEPT
{
	return g_Decompiler->Decompile(address, size);
}

_NODISCARD _CONSTEXPR20 const std::vector<PluginInterface>& Plugins() noexcept
{
	return g_Plugins;
}

void OnAttachCallback(LPON_ATTACH_CALLBACK callback) noexcept;
void OnDetachCallback(LPON_DETACH_CALLBACK callback) noexcept;

}

#endif // !TOKIO_ENGINE_H