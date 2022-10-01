#pragma once
#include "stdafx.h"
#include "ViewMemoryScan.h"
#include "GUI/MainView.h"
#include "GUI/Widgets/Widgets.hpp"

ViewMemoryScan::ViewMemoryScan()
{
	m_title = ICON_MEMORY_SCAN u8" Memory Scan";

	auto viewList = MainView::FindMultipleViewByClass<ViewMemoryScan>();
	if (viewList.size() > 0) m_title += " " + std::to_string(viewList.size() + 1);
}

void ViewMemoryScan::Render(bool& bOpen)
{
	if (!bOpen) return;

	ImGui::Begin(Title().c_str(), &bOpen);


	ImGui::End();
}
