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

#pragma once
#ifndef TOKIO_GUI_WIDGETS_HPP
#define TOKIO_GUI_WIDGETS_HPP

// TODO: FIX THE STRUCTURE OF THIS FILE


//#include <string>
//#include <vector>
//
//#include "imgui.hpp"

#include "WidgetsFlags.hpp"
#include "Resources/FontAwesomeImpl.h"

//#include "WTable.h"
//#include "WTreeTable.hpp"
//#include "WTextInput.hpp"
//#include "WPopup.h"




namespace Widgets
{

// Get ImGui unique name from a string and a pointer
// The pointer is usually a class that handles the widget
// Or it could be anything... as long as it is unique
_CONSTEXPR20 std::string GetUniqueName(const std::string& name, void* ptr)
{
	return name + "##" + std::to_string(reinterpret_cast<unsigned long long>(ptr));
}

template <typename T>
_CONSTEXPR20 ImGuiID GetUniqueID(T uint64_ptr)
{
	return ImHashData(&uint64_ptr, sizeof(T), 0);
}

}

#endif // !TOKIO_GUI_WIDGETS_HPP
