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
#include "stdafx.h"
#include "WPopup.h"
#include "Widgets.hpp"

namespace Widgets
{

void Popup::Setup(const Desc& desc)
{
	// must set the render callback
	assert(desc.RenderCallback != nullptr);

	m_desc = desc;
	if (m_desc.id == 0) m_desc.id = GetUniqueID(this);
	m_desc.name = GetUniqueName(m_desc.name, this);
}

void Popup::Render()
{
	if (m_nextOpen)
	{
		if ((m_desc.WidgetFlags & PopupFlags::PopupModal) == PopupFlags::PopupModal)
		{

			ImGui::OpenPopup(m_desc.name.c_str(), m_desc.OpenFlags);
		}
		else
		{
			ImGui::OpenPopupEx(m_desc.id, m_desc.OpenFlags);
		}
		m_nextOpen = false;
	}

	if ((m_desc.WidgetFlags & PopupFlags::PopupModal) == PopupFlags::PopupModal)
	{
		bool bOpen = true;
		m_isOpen = ImGui::BeginPopupModal(m_desc.name.c_str(), &bOpen, m_desc.Flags);
	}
	else
	{
		m_isOpen = ImGui::BeginPopupEx(m_desc.id, m_desc.Flags);
	}

	if (m_isOpen)
	{
		m_desc.RenderCallback(this, m_desc.OpenUserData, m_desc.RenderUserData);

		if ((m_desc.WidgetFlags & PopupFlags::CloseOnEscape) == PopupFlags::CloseOnEscape)
		{
			if (ImGui::IsKeyPressed(ImGuiKey_Escape)) ImGui::CloseCurrentPopup();
		}
		if (m_closePopupQueue) ImGui::CloseCurrentPopup();

		// set cursor pos to make the window auto resize to the initial size
		if (m_desc.initialSize.x != 0.f || m_desc.initialSize.y != 0.f)
			ImGui::SetCursorPos(m_desc.initialSize);

		ImGui::EndPopup();
	}

	m_closePopupQueue = false;
}

}