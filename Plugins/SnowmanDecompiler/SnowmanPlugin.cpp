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