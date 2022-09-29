#pragma once


class ViewModules : public BaseView
{
private:
	Widgets::Table m_table;							// main table
	Widgets::TextInput m_textFilter;				// text input for filtering modules
	std::vector<ModuleData> m_moduleList;

	static Widgets::Table::Execution
	TableRenderCallback(Widgets::Table* table, UINT index, void* UserData)
	{
		auto pThis = static_cast<ViewModules*>(UserData);
		auto& modData = pThis->m_moduleList[index];

		ImGui::Text("%llx", modData.baseAddress);

		ImGui::TableNextColumn();

		if (ImGui::TreeNode("%s", modData.moduleNameA.c_str()))
		{
			for (auto& exportData : modData.pe.dirs.exports)
			{
				//ImGui::TableSetColumnIndex(0);
				//ImGui::Text("%llx", modData.baseAddress + exportData.offset);
				//ImGui::TableSetColumnIndex(1);
				ImGui::Text("%s", exportData.name.c_str());
			}
			ImGui::TreePop();
		}

		return Widgets::Table::Execution::Continue;
	}

	static void TableInputCallback(Widgets::Table* table, INT index, void* UserData)
	{

	}
	static void TableSortCallback(Widgets::Table* table, UINT column, ImGuiSortDirection direction, void* UserData)
	{

	}
	static void TablePopupRenderCallback(Widgets::Table* table, UINT index, void* UserData)
	{

	}

public:
	ViewModules()
	{
		m_title = u8"🕹 Modules List";

		auto viewList = MainView::FindMultipleViewByClass<ViewScanner>();
		if (viewList.size() > 0) m_title += " " + std::to_string(viewList.size() + 1);


		Widgets::Table::Desc desc;
		desc.Name = "##TableAttachProc";

		desc.SortUserData = this;
		desc.InputUserData = this;
		desc.RenderUserData = this;
		desc.PopupRenderUserData = this;

		desc.SortCallback = TableSortCallback;
		desc.InputCallback = TableInputCallback;
		desc.RenderCallback = TableRenderCallback;
		desc.PopupRenderCallback = TablePopupRenderCallback;


		desc.Flags = ImGuiTableFlags_RowBg							| // two background mode
					 ImGuiTableFlags_Borders						| // row borders
					 ImGuiTableFlags_ScrollY						| // enable vertical scrool
					 ImGuiTableFlags_Hideable						| // hide the columns
					 ImGuiTableFlags_Sortable						| // enable sorting
					 ImGuiTableFlags_Resizable						| // not sure what this does
					 ImGuiTableFlags_Reorderable					; // re-order the coulmns

		m_table.Setup(desc);

		// flags for the address column
		auto colFlags = ImGuiTableColumnFlags_DefaultSort			| // default sort by address
						ImGuiTableColumnFlags_PreferSortAscending	; 

		m_table.AddColumn("Address", colFlags);
		m_table.AddColumn("Name");

		Widgets::TextInput::Desc tiDesc;
		tiDesc.Label = "##Filter";
		tiDesc.Hint = "Filter";
		m_textFilter.Setup(tiDesc);
	}

	void Render(bool& bOpen) override
	{
		if (!bOpen) return;

		ImGui::Begin(Title().c_str(), &bOpen);

		m_moduleList = Engine::GetModuleList();
		m_table.Render(static_cast<UINT>(m_moduleList.size()));

		ImGui::End();
	}
};
