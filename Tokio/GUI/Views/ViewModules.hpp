#pragma once


class ViewModules : public BaseView
{
private:
	class ModuleNode : public Widgets::TreeTable<ModuleNode>::NestedNode
	{
		using NestedNode::NestedNode;
	public:

		bool isModule = false;
		POINTER address = 0;

		// for modules
		std::string moduleNameA;
		std::string modulePathA;
		std::wstring moduleNameW;
		std::wstring modulePathW;
		PE_Data pe;

		// for symbols
		UINT ordinal = 0;
		std::string functionName;

		bool isHidden = false;		// true to hide this node, if the filter is on
		bool m_lastOpen = false;	// store the open state before start filtering, back it up if we don't filter anymore, only for modules

	};


	Widgets::TreeTable<ModuleNode> m_table;			// main table
	Widgets::TextInput m_textFilter;				// text input for filtering modules
	std::vector<ModuleNode> m_moduleList;

	static Widgets::TreeTable<ModuleNode>::Execution
	TableRenderCallback(
		Widgets::TreeTable<ModuleNode>* table,
		ModuleNode& node,
		size_t index,
		size_t level,
		void* UserData
	)
	{
		if (node.isHidden) return Widgets::TreeTable<ModuleNode>::Execution::Skip;

		auto pThis = static_cast<ViewModules*>(UserData);


		// Render the node as module
		if (node.isModule)
		{

			ImGui::PushFont(MainApplication::FontMonoBold);
			ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextAddressModule), "%llX", node.address);

			table->NextColumn();
			ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextModule), "%s", node.moduleNameA.c_str());
			ImGui::PopFont();
		}
		// Render it as symbol
		else
		{
			ImGui::PushFont(MainApplication::FontMono);
			ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextAddressModule), "%llX", node.address);

			table->NextColumn();
			ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextFunction), "%s", node.functionName.c_str());
			ImGui::PopFont();
		}


		return Widgets::TreeTable<ModuleNode>::Execution::Continue;
	}

	static void TableInputCallback(Widgets::TreeTable<ModuleNode>* table, INT index, void* UserData)
	{

	}
	static void TableSortCallback(Widgets::TreeTable<ModuleNode>* table, size_t column, ImGuiSortDirection direction, void* UserData)
	{

	}
	static void TablePopupRenderCallback(Widgets::TreeTable<ModuleNode>* table, size_t index, void* UserData)
	{

	}

	static void FilterEditCallback(Widgets::TextInput* tinput, ImGuiInputTextCallbackData* data, void* UserData)
	{
		ViewModules* pThis = static_cast<ViewModules*>(UserData);

		// the filter contains nothing
		if (data->BufTextLen == 0)
		{
			for (auto& modData : pThis->m_moduleList)
			{
				modData.m_open = modData.m_lastOpen;
				modData.isHidden = false;

				for (auto& symbolData : modData.m_childs)
				{
					symbolData.isHidden = false;
				}
			}
		}
		else
		{
			// store the open state on enabling the filter
			if (data->BufTextLen == 1)
			{
				for (auto& modData : pThis->m_moduleList)
					modData.m_lastOpen = modData.m_open;
			}


			std::string filter = std::string(data->Buf, data->BufTextLen);
			filter = common::BhStringLower(filter);

			for (auto& modData : pThis->m_moduleList)
			{
				bool isAnyChildVisible = false;

				for (auto& symbolData : modData.m_childs)
				{
					std::string lowerName = common::BhStringLower(symbolData.functionName);

					// hide it if couldn't find the filter in its name
					symbolData.isHidden = (lowerName.find(filter) == std::string::npos);

					isAnyChildVisible |= !symbolData.isHidden;
				}

				// if any of its symbol is visible, don't hide it
				if (isAnyChildVisible)
				{
					modData.isHidden = false;
					if (!modData.m_open) modData.m_open = true;
				}
				else
				{
					std::string lowerName = common::BhStringLower(modData.moduleNameA);
					modData.isHidden = (lowerName.find(filter) == std::string::npos);
				}
			}
		}
	}

public:
	ViewModules()
	{
		m_title = u8"🕹 Symbol List";

		auto viewList = MainView::FindMultipleViewByClass<ViewScanner>();
		if (viewList.size() > 0) m_title += " " + std::to_string(viewList.size() + 1);


		Widgets::TreeTable<ModuleNode>::Desc desc;
		desc.Name = "##TableModuleList";

		desc.SortUserData = this;
		desc.InputUserData = this;
		desc.RenderUserData = this;
		desc.PopupRenderUserData = this;

		desc.SortCallback = TableSortCallback;
		desc.InputCallback = TableInputCallback;
		desc.RenderCallback = TableRenderCallback;
		desc.PopupRenderCallback = TablePopupRenderCallback;

		//desc.WidgetFlags = Widgets::TableFlags::NoHeader;

		desc.Flags = ImGuiTableFlags_RowBg							| // two background mode
					 ImGuiTableFlags_BordersOuter					| // row borders
					 ImGuiTableFlags_ScrollY						| // enable vertical scrool
					 ImGuiTableFlags_Hideable						| // hide the columns
					 ImGuiTableFlags_Sortable						| // enable sorting
					 ImGuiTableFlags_Resizable						| // not sure what this does
					 ImGuiTableFlags_Reorderable					; // re-order the coulmns

		// need this for the tree node to span the full row
		//desc.ExtraFlags = ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX;

		m_table.Setup(desc);

		// flags for the address column
		auto colFlags = ImGuiTableColumnFlags_DefaultSort			| // default sort by address
						ImGuiTableColumnFlags_PreferSortAscending	; 

		m_table.AddColumn("Address");
		m_table.AddColumn("Symbol");

		Widgets::TextInput::Desc tiDesc;
		tiDesc.Label = "##Filter";
		tiDesc.Hint = "Filter";
		tiDesc.EditCallback = FilterEditCallback;
		tiDesc.EditUserData = this;
		m_textFilter.Setup(tiDesc);
	}

	void Render(bool& bOpen) override
	{
		if (!bOpen) return;

		if (m_moduleList.size() > 0)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 0,0 });
			ImGui::Begin(Title().c_str(), &bOpen);
			ImGui::PopStyleVar();

			ImGui::SetNextItemWidth(-1);
			m_textFilter.Render();

			m_table.Render(m_moduleList);
			ImGui::End();
		}
		else
		{
			ImGui::Begin(Title().c_str(), &bOpen);
			ImGui::TextDisabled("No modules to display");
			ImGui::End();
		}

	}

	// update the module data
	void Update(const std::vector<ModuleData>& modules)
	{
		m_moduleList.clear();
		m_moduleList.reserve(modules.size());

		for (auto& modData : modules)
		{
			ModuleNode& nodeModule = m_moduleList.emplace_back();

			nodeModule.isModule = true;
			nodeModule.address = modData.address;
			nodeModule.moduleNameA = modData.moduleNameA;
			nodeModule.moduleNameW = modData.moduleNameW;
			nodeModule.modulePathA = modData.modulePathA;
			nodeModule.modulePathW = modData.modulePathW;

			for (auto& exportData : modData.pe.dirs.exports)
			{
				ModuleNode& nodeSymbol = nodeModule.AddChild();

				nodeSymbol.isModule = false;
				nodeSymbol.ordinal = exportData.ordinal;
				nodeSymbol.address = nodeModule.address + exportData.offset;
				nodeSymbol.functionName = exportData.name;
			}
		}
	}
};
