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
}

void Popup::Render()
{
	m_isOpen = ImGui::BeginPopupEx(m_desc.id, m_desc.Flags);

	if (m_isOpen)
	{
		m_desc.RenderCallback(this, m_desc.OpenUserData, m_desc.RenderUserData);

		if (m_closePopupQueue)
		{
			ImGui::CloseCurrentPopup();
			m_closePopupQueue = false;
		}
		ImGui::EndPopup();
	}

	m_closePopupQueue = false;
}

}