#ifndef PLUGINS_SNOWMANDECOMPILER_SNOWMANPLUGIN_H
#define PLUGINS_SNOWMANDECOMPILER_SNOWMANPLUGIN_H

#include "Engine/Plugins/PluginsDef.h"
#include <memory>

#define PLUGIN_API __declspec(dllexport)

extern "C++"
{
	PLUGIN_API PluginInterface __stdcall PluginEntry(EngineInterface EngineInterface);
}



#endif // !PLUGINS_SNOWMANDECOMPILER_SNOWMANPLUGIN_H
