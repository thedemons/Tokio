#pragma once


class ViewScanner : public BaseView
{

public:
	ViewScanner()
	{
		m_title = u8"🔍 Memory Scan";

		auto viewList = MainView::FindMultipleViewByClass<ViewScanner>();
		if (viewList.size() > 0) m_title += " " + std::to_string(viewList.size() + 1);
	}

	void Render(bool& bOpen) override
	{
		if (!bOpen) return;

		ImGui::Begin(Title().c_str(), &bOpen);

		
		ImGui::End();
	}
};