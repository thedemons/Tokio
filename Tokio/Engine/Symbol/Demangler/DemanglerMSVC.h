#pragma once
#ifndef TOKIO_ENGINE_SYMBOL_DEMANGLER_MSVC_H
#define TOKIO_ENGINE_SYMBOL_DEMANGLER_MSVC_H

#include <string>

namespace Engine
{

std::string DemangleSymbolMSVC(const char* symbol);

}
#endif // !TOKIO_ENGINE_SYMBOL_DEMANGLER_MSVC_H