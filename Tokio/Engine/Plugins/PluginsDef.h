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