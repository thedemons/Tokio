#ifndef TOKIO_ENGINE_PLUGINS_PLUGIN_LOADER_H
#define TOKIO_ENGINE_PLUGINS_PLUGIN_LOADER_H

#include <vector>
#include "PluginsDef.h"

#include "Common/Exception.h"

namespace Engine
{
extern std::vector<PluginInterface> g_Plugins;
void LoadPlugins() EXCEPT;
void LoadDecompilerPlugin() noexcept;
}

#endif // !TOKIO_ENGINE_PLUGINS_PLUGIN_LOADER_H