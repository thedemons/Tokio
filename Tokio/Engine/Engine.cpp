#pragma once
#include "stdafx.h"
#include "Engine.h"
#include "Memory/Win32Memory.hpp"
#include "PEInfo/PEParser.hpp"


namespace Engine
{

// BASE MEMORY ENGINE
std::shared_ptr<BaseMemory> g_Memory = nullptr;

// TARGET PROCESS
std::shared_ptr<ProcessData> g_Target = nullptr;

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

	// parse PE header for each module in the target process
	// TODO: merge this with Win32Memory
	for (auto& modData : g_Target->modules)
	{
		if (auto peInfo = PEParser::GetPEInfo(modData.modulePathW); !peInfo.has_error())
		{
			modData.pe = peInfo.value();
		}
		else
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
}

_NODISCARD std::shared_ptr<ProcessData> Target()
{
	return g_Target;
}

_NODISCARD std::shared_ptr<BaseMemory> Memory()
{
	return g_Memory;
}

_NODISCARD auto ReadMem(POINTER src, void* dest, size_t size)->SafeResult(void)
{
	return g_Memory->Read(src, dest, size);
}

_NODISCARD auto WriteMem(POINTER dest, const void* src, size_t size)->SafeResult(void)
{
	return g_Memory->Write(dest, src, size);
}

void OnAttachCallback(LPON_ATTACH_CALLBACK callback)
{
	pAttachCallback = callback;
}
}