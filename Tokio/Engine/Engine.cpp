#pragma once
#include "stdafx.h"
#include "Engine.h"

#include "Common/Exception.h"

#include "Memory/Win32Memory.hpp"
#include "Symbol/Win32Symbol.h"
#include "Disassembler/ZydisDisassembler.h"

#include "Analyzer/TokioAnalyzer.h"


namespace Engine
{

LPON_ATTACH_CALLBACK pAttachCallback = nullptr;
LPON_DETACH_CALLBACK pDetachCallback = nullptr;

void Attach(PID pid) EXCEPT
{
	// detach the current target before attaching to a new process
	if (g_Target != nullptr) Detach();

	try
	{
		g_Memory       = std::make_shared<Win32Memory>();

		g_Target       = g_Memory->Attach(pid);
		g_Symbol       = std::make_shared<Win32Symbol>		(g_Target);
		g_Disassembler = std::make_shared<ZydisDisassembler>(g_Target);
		g_Analyzer     = std::make_shared<TokioAnalyzer>	(g_Target, g_Symbol, g_Memory, g_Disassembler);
		

	}
	catch (Tokio::Exception& e)
	{
		Detach();

		e += "Attach process failed";
		throw e;
	}

	if (pAttachCallback) pAttachCallback(g_Target);
}

void Detach() noexcept
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
void OnAttachCallback(LPON_ATTACH_CALLBACK callback) noexcept
{
	pAttachCallback = callback;
}

void OnDetachCallback(LPON_DETACH_CALLBACK callback) noexcept
{
	pDetachCallback = callback;
}

}