#pragma once
#include "stdafx.h"
#include "WTable.h"
#include "Widgets.hpp"

namespace Widgets
{

void Table::Setup(const Table::Desc& desc)
{
	// We must have a RenderCallback!
	assert(desc.RenderCallback != nullptr);

	// case 1: The SortCallback is set but there's no Sortable flag
	// case 2: The Sortable flag is set but there's no SortCallback
	assert(bool(desc.Flags & ImGuiTableFlags_Sortable) == bool(desc.SortCallback));

	m_desc = desc;
	m_desc.Name = GetUniqueName(m_desc.Name, this);

	if (m_desc.PopupRenderCallback != nullptr)
	{
		Popup::Desc popupDesc;
		popupDesc.RenderCallback = PopupRenderCallback;
		popupDesc.RenderUserData = this;
		m_popup.Setup(popupDesc);
	}
}

void Table::AddColumn(const std::string& Name, ImGuiTableColumnFlags Flags, float width)
{
	ColumnData column;
	column.Name = Name;
	column.Width = width;
	column.Flags = Flags;
	column.Index = static_cast<size_t>(m_columns.size());

	m_columns.push_back(column);
}

void Table::HandleRowInput(ImGuiTable* table, size_t index)
{
	// get the row rect
	ImRect row_rect(
		table->WorkRect.Min.x,
		table->RowPosY1,
		table->WorkRect.Max.x,
		table->RowPosY2
	);
	row_rect.ClipWith(table->BgClipRect);

	if (row_rect.Min.y >= row_rect.Max.y) return;

	bool bHover =
		ImGui::IsMouseHoveringRect(row_rect.Min, row_rect.Max, false) &&
		ImGui::IsWindowFocused(ImGuiFocusedFlags_None) &&
		!ImGui::IsAnyItemHovered();

	bool bLBtnDown = ImGui::IsMouseDown(0);
	bool bLBtnClicked = ImGui::IsMouseReleased(0);
	bool bItemSelected = IsItemSelected(index);

	ImU32 color = IM_COL32_DISABLE;

	if (bHover)
	{
		m_hoveredIndex = index;

		if (bLBtnDown)
			color = ImGui::GetColorU32(ImGuiCol_TableRowClicked);
		else if (bItemSelected)
			color = ImGui::GetColorU32(ImGuiCol_TableRowActive);
		else
			color = ImGui::GetColorU32(ImGuiCol_TableRowHovered);

		if (!bItemSelected && bLBtnClicked)
		{
			// TODO: handle multiple selection
			AddSelectedItem(index);
		}
	}
	else if (bItemSelected)
	{
		color = ImGui::GetColorU32(ImGuiCol_TableRowActive);
	}


	// override row bg color (imgui_tables.cpp line 1771)
	table->RowBgColor[1] = color;
}

void Table::HandlePopup()
{
	if (m_desc.PopupRenderCallback == nullptr) return;

	m_popup.Render();

	if (m_hoveredIndex == UPTR_UNDEFINED) return;

	// if the user right clicked a row
	if (ImGui::IsMouseReleased(1))
	{
		size_t index = static_cast<size_t>(m_hoveredIndex);

		// the user clicked outside of any selected item
		if (!IsItemSelected(index))
		{
			// we clear the selected items, then set the current item to be selected
			ClearSelectedItems();
			AddSelectedItem(index);
		}

		// open the popup
		m_popup.Open(reinterpret_cast<void*>(index));
	}

}

void Table::PopupRenderCallback(Popup* popup, void* OpenUserData, void* UserData)
{
	Table* pThis = static_cast<Table*>(UserData);

	// why don't we have a popup render callback?
	assert(pThis->m_desc.PopupRenderCallback != nullptr);

	// forward the callback
	// OpenUserData is the index hovered when the user right-clicked
	size_t index = reinterpret_cast<size_t>(OpenUserData);
	pThis->m_desc.PopupRenderCallback(pThis, index, pThis->m_desc.PopupRenderUserData);
}

void Table::AddSelectedItem(size_t index)
{
	if (!m_desc.IsMultiSelection)
	{
		if (m_selected.size() > 0) m_selected[0] = index;
		else m_selected.push_back(index);
	}
	else
	{
		// TODO: Add multi selection support
	}
}


// nMaxItem: enforce the row/item limit
// bypass the Execution::Continue returned from the RenderCallback
void Table::Render(size_t nMaxItem, ImVec2 Size)
{
	ImGuiContext& g = *GImGui;

	if (ImGui::BeginTable(m_desc.Name.c_str(), static_cast<int>(m_columns.size()), m_desc.Flags, Size))
	{
		auto* table = g.CurrentTable;

		for (auto& column : m_columns)
		{
			ImGui::TableSetupColumn(column.Name.c_str(), column.Flags, column.Width);
		}

		// draw table header
		if (!(m_desc.WidgetFlags & TableFlags::NoHeader))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableHeadersRow();
		}

		// check for sort events
		// this code is straight up copied from the imgui_demo.cpp (why SpecsDirty?)
		if (m_desc.Flags & ImGuiTableFlags_Sortable)
		{
			if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs(); sorts_specs->SpecsDirty)
			{
				sorts_specs->SpecsDirty = false;
				Sort(
					sorts_specs->Specs->ColumnIndex,
					sorts_specs->Specs->SortDirection
				);
			}
		}

		// reset hover index before we rendering any rows
		// notice that the m_currentHoverIndex of the last
		// frame will still be available for SortCallback
		// to use, we might use it in the future
		m_hoveredIndex = UPTR_UNDEFINED;

		//table->Flags |= m_desc.ExtraFlags;

		Execution state = Execution::Continue;

		for (size_t i = 0; state != Execution::Stop && i < nMaxItem; i++)
		{
			if (state != Execution::Skip)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
			}

			state = m_desc.RenderCallback(this, i, m_desc.RenderUserData);

			if (state != Execution::Skip)
			{
				// if we don't jump to the last column
				// it will fail to calculate the rect size
				// just in case we didn't render enough
				// columns in the user RenderCallback
				ImGui::TableSetColumnIndex(static_cast<int>(m_columns.size()) - 1);


				// TODO: Check for active items and
				// ImGui::IsWindowFocused(ImGuiFocusedFlags_None) && !ImGui::IsAnyItemHovered()
				// for not to call HandleRowInput multiple times
				HandleRowInput(table, i);
			}
		}

		// it means that we begin a table row
		// and never used it, if we don't "destroy" it
		// it will glitched into the table as a tiny teeny row
		// i don't know if it is appropriate to do this
		// this is EXTREMELY hacky and dirty, changing the internal
		// of imgui, but it works fine so be it!
		if (state == Execution::Skip)
		{
			// ImGui::TableNextRow added the offset in imgui_tables.cpp#1691
			// so we must subtract it, not sure what the ImMax does in #1692
			// but apparently we didn't need to revert it
			table->RowPosY2 -= table->CellPaddingY * 2.0f;

			// imgui_tables.cpp#1705
			// it doesn't make any difference if we don't subtract it
			// but still a good idea to do so
			table->CurrentRow--;

			// assertion on imgui_tables.cpp#1228
			// don't know what it is for
			table->InnerWindow->DC.CursorPos.y = table->RowPosY2;

			// imgui_tables.cpp#1214
			// let's not finalize the row if we weren't inside any eh?
			table->IsInsideRow = false;
		}

		// check if any columns is hovered
		m_hoveredColumn = UPTR_UNDEFINED;
		for (auto& column : table->Columns)
		{
			if (column.Flags & ImGuiTableColumnFlags_IsHovered)
			{
				m_hoveredColumn = static_cast<size_t>(&column - &table->Columns[0]);
				break;
			}
		}

		ImGui::EndTable();

		// call input callback
		if (m_desc.InputCallback)
			m_desc.InputCallback(this, m_hoveredIndex, m_desc.InputUserData);

		HandlePopup();
	}
}

};