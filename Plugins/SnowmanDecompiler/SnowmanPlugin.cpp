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

#include "stdafx.h"
#include "SnowmanPlugin.h"
#include "SnowmanDecompiler.h"


std::shared_ptr<Engine::BaseDecompiler>
PluginInit(const std::shared_ptr<ProcessData>& target)
{
	return std::make_shared<Engine::SnowmanDecompiler>(target);
}

PluginInterface __stdcall PluginEntry(EngineInterface EngineInterface)
{
	// need this to export unmangled symbol
	#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	Engine::EInterface = EngineInterface;

	PluginInterface interface;
	interface.type = PluginType::Decompiler;

	interface.archs.emplace_back(Architecture::x86);
	interface.archs.emplace_back(Architecture::x86_64);

	interface.name = "Snowman Decompiler";
	interface.version = "v1.0.0";

	interface.pInitFunction = PluginInit;

	return interface;
}