#pragma once
#ifndef IMGUI_API

// for ImVec math operations
#define IMGUI_DEFINE_MATH_OPERATORS

// disable ImGui's debugging tools
#define IMGUI_DISABLE_DEBUG_TOOLS

#pragma warning (push, 4)
#pragma warning( suppress : 4005)
#include "GUI/ImGui/imgui.h"
#include "GUI/ImGui/imgui_internal.h"
#include "GUI/ImGui/Impl/imgui_impl_dx11.h"
#include "GUI/ImGui/Impl/imgui_impl_win32.h"
#pragma warning (pop)

#endif // IMGUI_API
