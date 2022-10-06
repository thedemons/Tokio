#pragma once
#include "stdafx.h"
#include "ViewSymbolList.h"
#include "ViewDisassembler.h"
#include "Settings.h"


ViewSymbolList::SymbolTable::Execution
ViewSymbolList::TableRenderCallback(
	SymbolTable* table,
	ModuleNode& node,
	size_t index,
	size_t level,
	void* UserData
)
{
	UNUSED(index);
	UNUSED(level);
	UNUSED(UserData);

	if (node.isHidden) return SymbolTable::Execution::Skip;



	// Render the node as module
	if (node.isModule)
	{

		ImGui::PushFont(MainApplication::FontMonoBold);
		ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextAddressModule), "%llX", node.address);

		table->NextColumn();
		ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextModule), "%s", node.moduleNameA.c_str());
		//ImGui::SameLine();
		//ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextModule), "%s", node.modulePathA.c_str());
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

void ViewSymbolList::TableSortCallback(SymbolTable* table, size_t column, ImGuiSortDirection direction, void* UserData)
{
	UNUSED(table);

	ViewSymbolList* pThis = static_cast<ViewSymbolList*>(UserData);
	bool isAscending = direction == ImGuiSortDirection_Ascending;

	// sort by module address
	auto sortByAddress = [=](const ModuleNode& a, const ModuleNode& b) -> bool
	{
		return isAscending ? a.address < b.address : a.address > b.address;
	};

	// sort by module name
	auto sortByName = [=](const ModuleNode& a, const ModuleNode& b) -> bool
	{
		int cmp = std::strcmp(a.moduleNameA.c_str(), b.moduleNameA.c_str());
		if (cmp == 0) return sortByAddress(a, b);

		return isAscending ? cmp < 0 : cmp > 0;
	};

	// TODO: Should we have a sort by symbol feature? That would be a lot of work to do

	std::function sortFunction = sortByAddress;
	if (column == 1) sortFunction = sortByName;

	std::sort(pThis->m_moduleList.begin(), pThis->m_moduleList.end(), sortFunction);

}

void ViewSymbolList::TablePopupRenderCallback(SymbolTable* table, ModuleNode* node, size_t index, void* UserData)
{
	UNUSED(table);
	UNUSED(index);
	UNUSED(UserData);

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
		else
		{
			ImGui::PushFont(MainApplication::FontMonoRegular);
			ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextAddressModule), "%llX", node->address);

			ImGui::SameLine();
			ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextFunction), "%s", node->functionName.c_str());

			ImGui::PopFont();
		}

		ImGui::Separator();
		Settings::shortcuts.SymListCopyAddress.RenderInPopup();

		ImGui::Separator();
		Settings::shortcuts.SymListRefresh.RenderInPopup();
		Settings::shortcuts.SymListAddToWatchList.RenderInPopup();

		ImGui::Separator();
		Settings::shortcuts.SymListOpenInDisassembler.RenderInPopup();
		Settings::shortcuts.SymListOpenInMemoryView.RenderInPopup();

	}
}

void ViewSymbolList::FilterEditCallback(Widgets::TextInput* tinput, ImGuiInputTextCallbackData* data, void* UserData)
{
	UNUSED(tinput);

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


		std::string filter = std::string(data->Buf, static_cast<size_t>(data->BufTextLen));
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
				std::string functionLowerName = common::BhStringLower(symbolData.functionName);

				// hide it if couldn't find the filter in its name
				symbolData.isHidden = (functionLowerName.find(filter) == std::string::npos);

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
	m_title = MainView::GetViewTitle<ViewSymbolList>(ICON_SYMBOL_LIST u8" Symbol List");

	SymbolTable::Desc desc;
	desc.Name = "##TableModuleList";

	desc.SortUserData = this;
	desc.RenderUserData = this;
	desc.PopupRenderUserData = this;

	desc.SortCallback = TableSortCallback;
	desc.RenderCallback = TableRenderCallback;
	desc.PopupRenderCallback = TablePopupRenderCallback;

	//desc.WidgetFlags = Widgets::TableFlags::NoHeader;

	desc.Flags = ImGuiTableFlags_RowBg        | // two background mode
		         ImGuiTableFlags_BordersOuter | // row borders
		         ImGuiTableFlags_ScrollY      | // enable vertical scrool
		         ImGuiTableFlags_Hideable     | // hide the columns
		         ImGuiTableFlags_Sortable     | // enable sorting
		         ImGuiTableFlags_Resizable    | // not sure what this does
		         ImGuiTableFlags_Reorderable  ; // re-order the coulmns

// need this for the tree node to span the full row
//desc.ExtraFlags = ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX;

	m_table.Setup(desc);

	m_table.AddColumn("Address", ImGuiTableColumnFlags_WidthFixed, 140.f);
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

		//if (Settings::shortcuts.SymListOpenInDisassembler.IsPressedInWindow())
		//{
		//	if (auto selectedItems = m_table.GetSelectedItems(); selectedItems.size() > 0)
		//	{
		//		auto* node = m_table.GetNodeAtIndex(selectedItems[0]);
		//		auto disasmView = MainView::FindViewByClass<ViewDisassembler>();
		//		if (node && disasmView.has_value())
		//		{
		//			disasmView.value().pView->GoToAddress(node->address);
		//		}
		//	}
		//}

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
void ViewSymbolList::OnAttach(const std::shared_ptr<ProcessData>& targetProcess)
{
	for (auto& modData : targetProcess->modules)
	{
		ModuleNode& nodeModule = m_moduleList.emplace_back();

		nodeModule.isModule = true;
		nodeModule.address = modData.base;
		nodeModule.moduleNameA = modData.moduleNameA;
		nodeModule.moduleNameW = modData.moduleNameW;
		nodeModule.modulePathA = modData.modulePathA;
		nodeModule.modulePathW = modData.modulePathW;

		for (auto& exportData : modData.exports)
		{
			ModuleNode& nodeSymbol = nodeModule.AddChild();

			nodeSymbol.isModule = false;
			nodeSymbol.ordinal = exportData.ordinal;
			nodeSymbol.address = nodeModule.address + exportData.offset;
			nodeSymbol.functionName = exportData.name;
		}
	}
}

void ViewSymbolList::OnDetach()
{
	m_moduleList.clear();
}