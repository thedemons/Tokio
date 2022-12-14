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

PluginInterface& LoadDllPlugin(std::filesystem::path pluginDir) EXCEPT
{
    std::error_code ec;
    std::filesystem::current_path(pluginDir, ec);

    if (ec.value())
    {
        throw Tokio::Exception("LoadDllPlugin() could not set current working directory"s);
    }

    auto pluginName = pluginDir.filename();
    auto pluginFilePath = pluginName.concat(L".dll");

    if (!std::filesystem::exists(pluginFilePath))
    {
        throw Tokio::Exception("Found extension directory named \"%s\" but couldn't find \"%s\"\n",
            pluginName.string().c_str(), pluginFilePath.string().c_str());
    }


    HMODULE hModule = LoadLibraryW(pluginFilePath.c_str());
    if (hModule == nullptr)
    {
        throw Tokio::Exception(Tokio::Exception::Type::WinAPI,
            "Failed to load the plugin %s", Tokio::String(pluginFilePath).c_str());
    }

    PLUGIN_ENTRY pEntry = reinterpret_cast<PLUGIN_ENTRY>(GetProcAddress(hModule, "PluginEntry"));

    if (pEntry == nullptr)
    {
        throw Tokio::Exception(Tokio::Exception::Type::WinAPI,
            "Could not find the entry point for plugin %s", Tokio::String(pluginFilePath).c_str());
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
    auto currentWorkingDir = std::filesystem::current_path();

    try
    {
        std::wstring pluginsDir = L"Plugins\\";
        for (const auto& entry : std::filesystem::directory_iterator(pluginsDir))
        {
            if (!entry.is_directory()) continue;

            try
            {
                PluginInterface& plugin = LoadDllPlugin(entry.path());
                Tokio::Log("Loaded plugin: %s %s\n", plugin.name.c_str(), plugin.version.c_str());
            }
            catch (Tokio::Exception& e)
            {
                e.Log();
            }
        }
    }
    catch (std::filesystem::filesystem_error&)
    {
        // no plugins folder
    }

    std::filesystem::current_path(currentWorkingDir);
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