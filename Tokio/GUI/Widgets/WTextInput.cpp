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
#include "WTextInput.h"
#include "Widgets.hpp"


namespace Widgets
{
int TextInput::ResizeCallback(ImGuiInputTextCallbackData* data)
{
	TextInput* pThis = static_cast<TextInput*>(data->UserData);

	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		pThis->m_buffer.resize(static_cast<size_t>(data->BufSize));
		data->Buf = pThis->m_buffer.data();
		pThis->m_privateData = *data;
	}
	else
	{
		pThis->m_privateData = *data;

		if (pThis->m_desc.EditCallback) pThis->m_desc.EditCallback(pThis, data, pThis->m_desc.EditUserData);
	}


	return 0;
}

void TextInput::Setup(const Desc& desc)
{
	m_desc = desc;
	m_desc.Label = GetUniqueName(m_desc.Label, this);
}

bool TextInput::Render(const ImVec2& size)
{
	bool result = ImGui::InputTextEx(
		m_desc.Label.c_str(),
		m_desc.Hint.c_str(),
		m_buffer.data(),
		static_cast<int>(m_buffer.size()),
		size,
		m_desc.Flags | ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CallbackEdit,
		ResizeCallback,
		this
	);

	if (m_nextFocus)
	{
		ImGui::SetKeyboardFocusHere(-1);
		//ImGui::ActivateItem(ImGui::GetItemID());
		//ImGui::SetFocusID(ImGui::GetItemID(), ImGui::GetCurrentWindow());
		m_nextFocus = false;
	}
	return result;
}
}