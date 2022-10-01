#pragma once
#include "stdafx.h"
#include "Engine.h"
#include "Memory/Win32Memory.hpp"
#include "PEInfo/PEParser.hpp"
#include "Disassembler/ZydisDisassembler.h"


namespace Engine
{
// BASE MEMORY ENGINE
std::shared_ptr<BaseMemory> g_Memory = nullptr;

// TARGET PROCESS
std::shared_ptr<ProcessData> g_Target = nullptr;

// Disassemble ENGINE
std::shared_ptr<BaseDisassembler> g_Disassembler = nullptr;

// On process attach callback
LPON_ATTACH_CALLBACK pAttachCallback = nullptr;

_NODISCARD auto Attach(DWORD pid) -> SafeResult(std::shared_ptr<ProcessData>)
{
	// detach the current target before attaching to a new proce
	if (g_Target != nullptr) Detach();

	// init the memory engine
	g_Memory = std::make_shared<Win32Memory>();

	auto result = g_Memory->Attach(pid);
	if (result.has_error())
	{
		g_Memory.reset();
		return result;
	}

	g_Target = result.value();

	// init the disassembler
	g_Disassembler = std::make_shared<ZydisDisassembler>();

	// parse PE header for each module in the target process
	// TODO: merge this with Win32Memory
	for (auto& modData : g_Target->modules)
	{
		auto peInfo = PEParser::GetPEInfo(modData);

		if (peInfo.has_error())
		{
			peInfo.error().show(L"Error while parsing PE header");
		}

	}

	if (pAttachCallback) pAttachCallback(g_Target);

	return g_Target;
}

void Detach()
{
	g_Memory->Detach();
	g_Memory.reset();
	g_Target.reset();
	g_Disassembler.reset();
}

bool IsAttached()
{
	return g_Target != nullptr;
}

_NODISCARD std::shared_ptr<ProcessData> Target()
{
	return g_Target;
}

_NODISCARD std::shared_ptr<BaseMemory> Memory()
{
	return g_Memory;
}

_NODISCARD std::shared_ptr<BaseDisassembler> Disassembler()
{
	return g_Disassembler;
}

_NODISCARD auto ReadMem(POINTER src, void* dest, size_t size)->SafeResult(void)
{
	return g_Memory->Read(src, dest, size);
}

_NODISCARD auto WriteMem(POINTER dest, const void* src, size_t size)->SafeResult(void)
{
	return g_Memory->Write(dest, src, size);
}

// TODO: Make a list of callbacks, not just one
void OnAttachCallback(LPON_ATTACH_CALLBACK callback)
{
	pAttachCallback = callback;
}
}