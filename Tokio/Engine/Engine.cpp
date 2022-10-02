#pragma once
#include "stdafx.h"
#include "Engine.h"
#include "Memory/Win32Memory.hpp"
#include "Symbol/Win32Symbol.h"
#include "Disassembler/ZydisDisassembler.h"


namespace Engine
{
// On process attach callback
LPON_ATTACH_CALLBACK pAttachCallback = nullptr;

_NODISCARD auto Attach(DWORD pid) -> SafeResult(std::shared_ptr<ProcessData>)
{
	// detach the current target before attaching to a new proce
	if (g_Target != nullptr) Detach();

	// init the memory engine
	g_Memory = std::make_shared<Win32Memory>();

	if (auto result = g_Memory->Attach(pid); result.has_error())
	{
		Detach();
		return result;
	}
	else
	{
		g_Target = result.value();
	}


	// init the symbol and disassembler engine
	g_Symbol = std::make_shared<Win32Symbol>(g_Target);
	g_Disassembler = std::make_shared<ZydisDisassembler>(g_Target);

	if (auto result = g_Symbol->Update(); result.has_error())
	{
		Detach();
		RESULT_FORWARD(result);
	}

	if (pAttachCallback) pAttachCallback(g_Target);

	return g_Target;
}

void Detach()
{
	if (g_Disassembler != nullptr) g_Disassembler.reset();
	if (g_Symbol       != nullptr) g_Symbol.reset();
	if (g_Target       != nullptr) g_Target.reset();

	if (g_Memory != nullptr)
	{
		g_Memory->Detach();
		g_Memory.reset();
	}
	
}

bool IsAttached()
{
	return g_Target != nullptr;
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