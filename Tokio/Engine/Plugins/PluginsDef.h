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

#ifndef TOKIO_ENGINE_PLUGINS_DEF_H
#define TOKIO_ENGINE_PLUGINS_DEF_H

#include "Engine/EngineDef.h"
#include "Engine/Memory/BaseMemory.h"
#include "Engine/Symbol/BaseSymbol.h"
#include "Engine/Disassembler/BaseDisassembler.h"
#include "Engine/Decompiler/BaseDecompiler.h"


#include <memory>

namespace Engine
{

typedef std::shared_ptr<BaseMemory>      (*PLUGIN_MEMORY_INIT      )(); // the memory init should not throw any exceptions
typedef std::shared_ptr<BaseSymbol>      (*PLUGIN_SYMBOL_INIT      )(const std::shared_ptr<ProcessData>& target);
typedef std::shared_ptr<BaseDisassembler>(*PLUGIN_DISASSEMBLER_INIT)(const std::shared_ptr<ProcessData>& target);
typedef std::shared_ptr<BaseDecompiler>  (*PLUGIN_DECOMPILER_INIT  )(const std::shared_ptr<ProcessData>& target);

}

// might return nullptr if the engine didn't have one of these properties
typedef std::shared_ptr<ProcessData>             (*ENGINE_GET_TARGET)      ();
typedef std::shared_ptr<Engine::BaseMemory>      (*ENGINE_GET_MEMORY)      ();
typedef std::shared_ptr<Engine::BaseSymbol>      (*ENGINE_GET_SYMBOL)      ();
typedef std::shared_ptr<Engine::BaseDisassembler>(*ENGINE_GET_DISASSEMBLER)();
typedef std::shared_ptr<Engine::BaseDecompiler>  (*ENGINE_GET_DECOMPILER)  ();

struct EngineInterface
{
	ENGINE_GET_TARGET       pGetTarget;
	ENGINE_GET_MEMORY       pGetMemory;
	ENGINE_GET_SYMBOL       pGetSymbol;
	ENGINE_GET_DISASSEMBLER pGetDisassembler;
	ENGINE_GET_DECOMPILER   pGetDecompiler;
};

enum PluginType
{
	Memory,
	Symbol,
	Disassembler,
	Decompiler
};

struct PluginInterface
{
	PluginType type;					// type of the plugin
	std::vector<Architecture> archs;	// the architectures this plugin supported
	std::string name;					// the name of the plugin
	std::string version;				// the version of the plugin, in string
	void* pInitFunction;				// init function for this plugin, for instance: PLUGIN_DISASSEMBLER_INIT for PluginType::Disassembler
};

// the entry point for the plugin.dll
typedef PluginInterface (__stdcall *PLUGIN_ENTRY)(EngineInterface EngineInterface);

#endif // !TOKIO_ENGINE_PLUGINS_DEF_H