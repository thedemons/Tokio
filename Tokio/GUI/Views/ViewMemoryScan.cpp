#pragma once
#include "stdafx.h"
#include "ViewMemoryScan.h"
#include "GUI/MainView.h"
#include "GUI/Widgets/Widgets.hpp"

ViewMemoryScan::ViewMemoryScan()
{
	m_title = MainView::GetViewTitle<ViewMemoryScan>(ICON_MEMORY_SCAN u8" Memory Scan");
}

void ViewMemoryScan::Render(bool& bOpen)
{
	if (!bOpen) return;

	ImGui::Begin(Title().c_str(), &bOpen);


	ImGui::End();
}
