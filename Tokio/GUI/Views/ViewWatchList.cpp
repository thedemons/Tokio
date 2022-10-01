#pragma once
#include "stdafx.h"
#include "ViewWatchList.h"
#include "GUI/Widgets/Widgets.hpp"


ViewWatchList::ViewWatchList()
{
	m_title = ICON_WATCH_LIST u8" Watch List";

	auto viewList = MainView::FindMultipleViewByClass<ViewWatchList>();
	if (viewList.size() > 0) m_title += " " + std::to_string(viewList.size() + 1);
}

void ViewWatchList::Render(bool& bOpen)
{
	if (!bOpen) return;

	ImGui::Begin(Title().c_str(), &bOpen);
	ImGui::End();
}