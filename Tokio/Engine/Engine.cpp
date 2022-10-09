/*
 * Project Tokio
 * Author: thedemons
 * Github: thedemons/Tokio
 * Made:   With love
 *
 * License: MIT License
 *
 * Copyright(c) 2022 thedemons
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#include "stdafx.h"
#include "Engine.h"

#include "Common/Exception.h"

#include "Memory/Win32Memory.hpp"
#include "Symbol/Win32Symbol.h"
#include "Disassembler/ZydisDisassembler.h"
#include "Analyzer/TokioAnalyzer.h"

#include "Plugins/PluginLoader.h"


namespace Engine
{

LPON_ATTACH_CALLBACK pAttachCallback = nullptr;
LPON_DETACH_CALLBACK pDetachCallback = nullptr;

void Init() noexcept
{
	try
	{
		LoadPlugins();
	}
	catch (Tokio::Exception& e)
	{
		e.Log("Faile to load plugins");
	}

}

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
		
		LoadDecompilerPlugin();
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
	if (g_Decompiler   != nullptr) g_Decompiler.reset();
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