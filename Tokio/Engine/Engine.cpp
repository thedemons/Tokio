#pragma once
#include "stdafx.h"
#include "Engine.h"
#include "Memory/Win32Memory.hpp"
#include "PEInfo/PEParser.hpp"


namespace Engine
{

Win32Memory g_MemWin32;

void Init()
{
	g_MemoryEngine = static_cast<BaseMemory*>(&g_MemWin32);
}

auto Attach(DWORD pid) -> SafeResult(void)
{
	auto result = g_MemoryEngine->Attach(pid);
	if (result.has_error()) return result;

	g_pid = g_MemoryEngine->GetPID();
	g_handle = g_MemoryEngine->GetHandle();

	if (auto result = g_MemoryEngine->GetModuleList(); !result.has_error())
	{
		g_moduleList = result.value();
	}

	for (auto& modData : g_moduleList)
	{
		if (auto peInfo = PEParser::GetPEInfo(modData.modulePathW); !peInfo.has_error())
		{
			modData.pe = peInfo.value();
		}
	}

	return result;
}

_CONSTEXPR20 void Detach()
{
	g_MemoryEngine->Detach();
}

constexpr BaseMemory* Memory()
{
	return g_MemoryEngine;
}

_CONSTEXPR20 auto ReadMem(POINTER src, void* dest, size_t size)->SafeResult(void)
{
	return g_MemoryEngine->Read(src, dest, size);
}

_CONSTEXPR20 auto WriteMem(POINTER dest, const void* src, size_t size)->SafeResult(void)
{
	return g_MemoryEngine->Write(dest, src, size);
}


_CONSTEXPR20 DWORD GetPID()
{
	return g_pid;
}

_CONSTEXPR20 HANDLE GetHandle()
{
	return g_handle;
}


}