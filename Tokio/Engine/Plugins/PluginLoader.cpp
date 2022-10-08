#include "stdafx.h"
#include "PluginLoader.h"

#include "Engine/Engine.h"

#include "Common/Exception.h"
#include "Common/StringHelper.h"
#include "Common/PathHelper.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <filesystem>

using namespace std::string_literals;

namespace Engine
{

PluginInterface& LoadDllPlugin(const std::wstring& dllPath) EXCEPT
{
    HMODULE hModule = LoadLibraryW(dllPath.c_str());
    if (hModule == nullptr)
    {
        throw Tokio::Exception(Tokio::Exception::Type::WinAPI,
            "Failed to load the plugin %s", Tokio::String(dllPath).c_str());
    }

    PLUGIN_ENTRY pEntry = reinterpret_cast<PLUGIN_ENTRY>(GetProcAddress(hModule, "PluginEntry"));

    if (pEntry == nullptr)
    {
        printf("%d\n", GetLastError());
        throw Tokio::Exception(Tokio::Exception::Type::WinAPI,
            "Could not find the entry point for plugin %s", Tokio::String(dllPath).c_str());
    }

    static const EngineInterface EInterface{
        Engine::Target,
        Engine::Memory,
        Engine::Symbol,
        Engine::Disassembler,
        Engine::Decompiler,
    };

    return g_Plugins.emplace_back(pEntry(EInterface));
}

void LoadPlugins() EXCEPT
{
    try
    {
        std::wstring pluginsDir = L"Plugins\\";
        for (const auto& entry : std::filesystem::directory_iterator(pluginsDir))
        {
            if (!entry.is_directory()) continue;

            auto pluginName = entry.path().filename();
            auto pluginPath = entry.path() / pluginName.concat(L".dll");
            
            if (!std::filesystem::exists(pluginPath))
            {
                Tokio::Log("Found extension directory named \"%s\" but couldn't find \"%s\"\n",
                    pluginName.string().c_str(), pluginPath.string().c_str());
            }
            else
            {
                try
                {
                    PluginInterface& plugin = LoadDllPlugin(pluginPath);
                    Tokio::Log("Loaded plugin: %s %s\n", plugin.name.c_str(), plugin.version.c_str());
                }
                catch (Tokio::Exception& e)
                {
                    e.Log();
                }
            }
        }
    }
    catch (std::filesystem::filesystem_error&)
    {
        // no plugins folder
    }
}

void LoadDecompilerPlugin() noexcept
{
    //g_Target->arch
    for (const PluginInterface& plugin : g_Plugins)
    {
        if (plugin.type != PluginType::Decompiler) continue;

        for (const Architecture& arch : plugin.archs)
        {
            if (arch != g_Target->arch) continue;

            PLUGIN_DECOMPILER_INIT pInit =
                reinterpret_cast<PLUGIN_DECOMPILER_INIT>(plugin.pInitFunction);

            g_Decompiler = pInit(g_Target);
            return;
        }
    }

    Tokio::Log("You don't have a decompiler for the current target");
}

}