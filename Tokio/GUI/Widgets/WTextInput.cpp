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