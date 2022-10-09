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

#ifndef TOKIO_GUI_SHORTCUT_H
#define TOKIO_GUI_SHORTCUT_H

#include "imgui.hpp"
#include "GUI/ImGui/custom/TokenizedText.h"

class ShortcutKey
{
private:
	// need to hold this key, for example: Ctrl, Shift,...
	ImGuiKey m_holdKey = ImGuiKey_None;

	// need to hold holdKey and press this key to activate
	ImGuiKey m_pressKey = ImGuiKey_None;

	// a represent name of the key
	std::string m_name;

	// shortcut description
	std::string m_description;

	// icon
	ImGui::TokenizedText m_icon;

	bool m_overridePress = false;

	void UpdateName();
public:
	ShortcutKey();
	ShortcutKey(const ImGuiKey press, const char* description = nullptr, const ImGui::TokenizedText* icon = nullptr);
	ShortcutKey(const ImGuiKey hold, const ImGuiKey press, const char* description = nullptr, const ImGui::TokenizedText* icon = nullptr);
	
	// NOTICE: if the item was clicked when rendering, the next 
	// IsPressed() or IsPressedInWindow() call will return true
	bool RenderInPopup(bool bEnabled = true);
	void RenderEdittor();

	// is the shortcut key was pressed
	_NODISCARD _CONSTEXPR20 bool IsPressed()
	{
		if (m_overridePress)
		{
			m_overridePress = false;
			return true;
		}

		if (m_holdKey != ImGuiKey_None)
			return ImGui::IsKeyDown(m_holdKey) && ImGui::IsKeyPressed(m_pressKey);

		return ImGui::IsKeyPressed(m_pressKey);
	}

	// is the shortcut key was pressed inside the current window, requires focusing
	_NODISCARD _CONSTEXPR20 bool IsPressedInWindow(ImGuiFocusedFlags Flags = ImGuiFocusedFlags_ChildWindows)
	{
		if (m_overridePress)
		{
			m_overridePress = false;
			return true;
		}

		return IsPressed() && ImGui::IsWindowFocused(Flags);
	}

	_NODISCARD _CONSTEXPR20 ImGuiKey HoldKey()
	{
		return m_holdKey;
	}

	_NODISCARD _CONSTEXPR20 ImGuiKey PressKey()
	{
		return m_pressKey;
	}

	_CONSTEXPR20 bool operator == (const ShortcutKey& other)
	{
		return m_holdKey == other.m_holdKey && m_pressKey == other.m_pressKey;
	}
};

#endif // !TOKIO_GUI_SHORTCUT_H
