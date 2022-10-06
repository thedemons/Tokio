#pragma once
#include "stdafx.h"
#include "ViewWatchList.h"
#include "GUI/Widgets/Widgets.hpp"


ViewWatchList::ViewWatchList()
{
	m_title = MainView::GetViewTitle<ViewWatchList>(ICON_WATCH_LIST u8" Watch List");
}

void ViewWatchList::Render(bool& bOpen)
{
	if (!bOpen) return;

	ImGui::Begin(Title().c_str(), &bOpen);
	ImGui::End();
}