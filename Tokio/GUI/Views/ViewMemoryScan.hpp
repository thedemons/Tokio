#pragma once


class ViewMemoryScan : public BaseView
{

public:
	ViewMemoryScan()
	{
		m_title = ICON_MEMORY_SCAN u8" Memory Scan";

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