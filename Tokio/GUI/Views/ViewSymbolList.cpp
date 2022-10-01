#pragma once
#include "stdafx.h"
#include "ViewSymbolList.h"
#include "ViewDisassembler.h"


ViewSymbolList::SymbolTable::Execution
ViewSymbolList::TableRenderCallback(
	SymbolTable* table,
	ModuleNode& node,
	size_t index,
	size_t level,
	void* UserData
)
{
	if (node.isHidden) return SymbolTable::Execution::Skip;

	auto pThis = static_cast<ViewSymbolList*>(UserData);


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
		ImGui::PushFont(MainApplication::FontMonoRegular);
		ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextAddressModule), "%llX", node.address);

		table->NextColumn();
		ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextFunction), "%s", node.functionName.c_str());

		ImGui::PopFont();
	}


	return SymbolTable::Execution::Continue;
}

void ViewSymbolList::TableInputCallback(SymbolTable* table, ModuleNode* node, size_t index, void* UserData)
{
	//printf("%d\n", index);
}

void ViewSymbolList::TableSortCallback(SymbolTable* table, size_t column, ImGuiSortDirection direction, void* UserData)
{

	//printf("%d\n", column);
}

void ViewSymbolList::TablePopupRenderCallback(SymbolTable* table, ModuleNode* node, size_t index, void* UserData)
{
	if (node != nullptr)
	{
		if (node->isModule)
		{

			ImGui::PushFont(MainApplication::FontMonoBold);
			ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextAddressModule), "%llX", node->address);

			ImGui::SameLine();
			ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextModule), "%s", node->moduleNameA.c_str());
			ImGui::PopFont();
		}
		// Render it as symbol
		else
		{
			ImGui::PushFont(MainApplication::FontMonoRegular);
			ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextAddressModule), "%llX", node->address);

			ImGui::SameLine();
			ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextFunction), "%s", node->functionName.c_str());

			ImGui::PopFont();
		}

		ImGui::Separator();
		ImGui::MenuItem(ICON_WATCH_LIST      u8" Add to Watch List");

		if (ImGui::MenuItem(ICON_DISASSEMBLER    u8" Open in Disassembler"))
		{
			auto disasmView = MainView::FindViewByClass<ViewDisassembler>();
			if (disasmView.has_value())
			{
				disasmView.value().pView->GoToAddress(node->address);
			}
		}

		ImGui::MenuItem(ICON_MEMORY_VIEW     u8" Open in Memory View", "Ctrl+B");
		ImGui::MenuItem(ICON_PE_VIEW         u8" Open in PE View");
	}
}

void ViewSymbolList::FilterEditCallback(Widgets::TextInput* tinput, ImGuiInputTextCallbackData* data, void* UserData)
{
	ViewSymbolList* pThis = static_cast<ViewSymbolList*>(UserData);

	// the filter contains nothing
	if (data->BufTextLen == 0)
	{
		for (auto& modData : pThis->m_moduleList)
		{
			modData.m_open = modData.m_lastOpen;
			modData.isHidden = false;

			for (auto& symbolData : modData.Childs())
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
			// check if the module pass the filter, if it does, show all its symbols
			std::string lowerName = common::BhStringLower(modData.moduleNameA);
			modData.isHidden = (lowerName.find(filter) == std::string::npos);

			if (!modData.isHidden)
			{
				modData.m_open = true;
				for (auto& symbolData : modData.Childs())
					symbolData.isHidden = false;

				continue;
			}

			bool isAnyChildVisible = false;

			for (auto& symbolData : modData.Childs())
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
		}
	}
}

ViewSymbolList::ViewSymbolList()
{
	m_title = ICON_SYMBOL_LIST u8" Symbol List";

	auto viewList = MainView::FindMultipleViewByClass<ViewSymbolList>();
	if (viewList.size() > 0) m_title += " " + std::to_string(viewList.size() + 1);


	SymbolTable::Desc desc;
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

	desc.Flags = ImGuiTableFlags_RowBg | // two background mode
		ImGuiTableFlags_BordersOuter | // row borders
		ImGuiTableFlags_ScrollY | // enable vertical scrool
		ImGuiTableFlags_Hideable | // hide the columns
		ImGuiTableFlags_Sortable | // enable sorting
		ImGuiTableFlags_Resizable | // not sure what this does
		ImGuiTableFlags_Reorderable; // re-order the coulmns

// need this for the tree node to span the full row
//desc.ExtraFlags = ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX;

	m_table.Setup(desc);

	// flags for the address column
	auto colFlags = ImGuiTableColumnFlags_DefaultSort | // default sort by address
		ImGuiTableColumnFlags_PreferSortAscending;

	m_table.AddColumn("Address");
	m_table.AddColumn("Symbol");

	Widgets::TextInput::Desc tiDesc;
	tiDesc.Label = "##Filter";
	tiDesc.Hint = "Filter";
	tiDesc.EditCallback = FilterEditCallback;
	tiDesc.EditUserData = this;
	m_textFilter.Setup(tiDesc);
}

void ViewSymbolList::Render(bool& bOpen) 
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
void ViewSymbolList::Update(const std::shared_ptr<ProcessData>& targetProcess)
{
	m_moduleList.clear();
	m_moduleList.reserve(targetProcess->modules.size());

	for (auto& modData : targetProcess->modules)
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
