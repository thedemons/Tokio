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