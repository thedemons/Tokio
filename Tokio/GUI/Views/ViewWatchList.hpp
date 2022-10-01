#pragma once
#include "GUI/Widgets/Widgets.hpp"

class ViewWatchList : public BaseView
{
public:
	ViewWatchList()
	{
		m_title = ICON_WATCH_LIST u8" Watch List";

		auto viewList = MainView::FindMultipleViewByClass<ViewMemoryScan>();
		if (viewList.size() > 0) m_title += " " + std::to_string(viewList.size() + 1);
	}

	void Render(bool& bOpen) override
	{
		if (!bOpen) return;

		ImGui::Begin(Title().c_str(), &bOpen);
		ImGui::End();
	}
};