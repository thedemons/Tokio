#pragma once
#include "stdafx.h"
#include "Engine.h"

#include "Memory/Win32Memory.hpp"
#include "Symbol/Win32Symbol.h"
#include "Disassembler/ZydisDisassembler.h"

#include "Analyzer/TokioAnalyzer.h"


namespace Engine
{
// On process attach callback
LPON_ATTACH_CALLBACK pAttachCallback = nullptr;
LPON_DETACH_CALLBACK pDetachCallback = nullptr;

_NODISCARD auto Attach(DWORD pid) -> SafeResult(std::shared_ptr<ProcessData>)
{
	// detach the current target before attaching to a new proce
	if (g_Target != nullptr) Detach();

	// init the memory engine
	g_Memory = std::make_shared<Win32Memory>();

	if (auto result = g_Memory->Attach(pid); result.has_error())
	{
		assert(false && "Could not attach to the process");

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
	g_Analyzer = std::make_shared<TokioAnalyzer>(g_Target, g_Symbol, g_Memory, g_Disassembler);

	if (auto result = g_Symbol->Update(); result.has_error())
	{
		assert(false && "Symbol engine failed to update");

		Detach();
		RESULT_FORWARD(result);
	}

	if (pAttachCallback) pAttachCallback(g_Target);

	return g_Target;
}

void Detach()
{
	if (g_Symbol       != nullptr) g_Symbol.reset();
	if (g_Target       != nullptr) g_Target.reset();
	if (g_Disassembler != nullptr) g_Disassembler.reset();
	if (g_Analyzer     != nullptr) g_Analyzer.reset();

	if (g_Memory != nullptr)
	{
		g_Memory->Detach();
		g_Memory.reset();
	}
	
	if (pDetachCallback) pDetachCallback();
}



// TODO: Make a list of callbacks, not just one
void OnAttachCallback(LPON_ATTACH_CALLBACK callback)
{
	pAttachCallback = callback;
}

void OnDetachCallback(LPON_DETACH_CALLBACK callback)
{
	pDetachCallback = callback;
}

}