#pragma once
#include "WPopup.hpp"

namespace Widgets
{

class Table
{
public:
	// return type for the Render Callback
	// if the callback never return Execution::Stop,
	// it will be enforced by the nMaxItem passed to Table::Render()
	enum class Execution
	{
		Continue,	// continue rendering
		Stop,		// stop rendering
		Skip		// skip this item
	};

	typedef void      (*LPINPUT_CALLBACK)      (Table* table, size_t index, void* UserData);
	typedef Execution (*LPRENDER_CALLBACK)     (Table* table, size_t index, void* UserData);
	typedef void      (*LPPOPUPRENDER_CALLBACK)(Table* table, size_t index, void* UserData);
	typedef void      (*LPSORT_CALLBACK)       (Table* table, size_t column, ImGuiSortDirection direction, void* UserData);


	// Table setup description
	struct Desc
	{
		std::string Name;
		bool IsMultiSelection = false;
		ImGuiTableFlags Flags = ImGuiTableFlags_None;		// Flags for the table
		ImGuiTableFlags ExtraFlags = ImGuiTableFlags_None;	// Extra flag for row rendering

		TableFlags WidgetFlags = TableFlags::None;

		// You must handle the ordering of the selected
		// items by yourself, it won't be altered after the sort
		LPSORT_CALLBACK SortCallback = nullptr;

		// Called everyframe, after ImGui::EndTable
		// the index argument is the current hovered item
		// if it is -1 then no items is being hovered
		LPINPUT_CALLBACK InputCallback = nullptr;

		// MUST HAVE!! Callback to render your rows
		// return	Execution::Continue to keep rendering
		//			Execution::Stop to stop rendering
		//			Execution::Skip to skip the row
		LPRENDER_CALLBACK RenderCallback = nullptr;		

		// Use this if you want a popup openned when
		// the user right-clicked something
		// The index is of the item the user right-clicked
		// Render the popup in this callback
		LPPOPUPRENDER_CALLBACK PopupRenderCallback = nullptr;

		void* SortUserData = nullptr;
		void* InputUserData = nullptr;
		void* RenderUserData = nullptr;
		void* PopupRenderUserData = nullptr;
	};

private:
	struct ColumnData
	{
		size_t Index = 0;			// auto increase
		float Width = 0.f;		// initial width
		std::string Name;		// unique name
		ImGuiTableColumnFlags Flags = ImGuiTableColumnFlags_None;
	};

	// table description
	Desc m_desc;

	// selected items
	std::vector<size_t> m_selected;

	// column data, add a column using ::AddColumn
	std::vector<ColumnData> m_columns;

	// UPTR_UNDEFINED means no index is hovered
	size_t m_hoveredIndex = UPTR_UNDEFINED;

	// UPTR_UNDEFINED means no column is hovered
	size_t m_hoveredColumn = UPTR_UNDEFINED;

	// current sort column index
	size_t m_currentSortColumn = 0;

	// current sort direction
	ImGuiSortDirection m_currentSortDir = ImGuiSortDirection_Ascending;

	// popup widget for right click handling
	Popup m_popup;


public:
	void Setup(const Desc& desc)
	{
		// We must have a RenderCallback!
		assert(desc.RenderCallback != nullptr);

		// case 1: The SortCallback is set but there's no Sortable flag
		// case 2: The Sortable flag is set but there's no SortCallback
		assert(bool(desc.Flags & ImGuiTableFlags_Sortable) == bool(desc.SortCallback));

		m_desc = desc;
		m_desc.Name = GUIUtils::GetUniqueName(m_desc.Name, this);

		if (m_desc.PopupRenderCallback != nullptr)
		{
			Popup::Desc popupDesc;
			popupDesc.RenderCallback = PopupRenderCallback;
			popupDesc.RenderUserData = this;
			m_popup.Setup(popupDesc);
		}
	}

	void AddColumn(const std::string& Name, ImGuiTableColumnFlags Flags = ImGuiTableColumnFlags_None, float width = 0.f)
	{
		ColumnData column;
		column.Name = Name;
		column.Width = width;
		column.Flags = Flags;
		column.Index = static_cast<size_t>(m_columns.size());

		m_columns.push_back(column);
	}

private:
	void HandleRowInput(ImGuiTable* table, size_t index)
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

	void HandlePopup()
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

	static void PopupRenderCallback(Popup* popup, void* OpenUserData, void* UserData)
	{
		Table* pThis = static_cast<Table*>(UserData);

		// why don't we have a popup render callback?
		assert(pThis->m_desc.PopupRenderCallback != nullptr);

		// forward the callback
		// OpenUserData is the index hovered when the user right-clicked
		size_t index = reinterpret_cast<size_t>(OpenUserData);
		pThis->m_desc.PopupRenderCallback(pThis, index, pThis->m_desc.PopupRenderUserData);
	}

public:
	void AddSelectedItem(size_t index)
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

	_CONSTEXPR20 void ClearSelectedItems()
	{
		m_selected.clear();
	}

	_CONSTEXPR20 std::vector<size_t>& GetSelectedItems()
	{
		return m_selected;
	}

	_CONSTEXPR20 bool IsItemSelected(size_t index) const
	{
		if (!m_desc.IsMultiSelection)
		{
			return m_selected.size() > 0 ? m_selected[0] == index : false;
		}
		else
		{
			return std::find(m_selected.begin(), m_selected.end(), index) != m_selected.end();
		}
	}

	_CONSTEXPR20 size_t GetHoveredColumn() const
	{
		return m_hoveredColumn;
	}

	// Get the popup to interact with it
	// becareful not to break anything
	_CONSTEXPR20 Popup& GetPopup()
	{
		return m_popup;
	}

	// Manually trigger the sort callback
	// User must specify the m_desc.SortCallback beforehand
	_CONSTEXPR20 void Sort()
	{
		if (!m_desc.SortCallback) return;

		m_desc.SortCallback(
			this,
			m_currentSortColumn,
			m_currentSortDir,
			m_desc.SortUserData
		);
	}

	// Manually trigger the sort callback
	// User must specify the m_desc.SortCallback beforehand
	_CONSTEXPR20 void Sort(size_t column, ImGuiSortDirection direction)
	{
		if (!m_desc.SortCallback) return;

		m_currentSortColumn = column;
		m_currentSortDir = direction;

		return Sort();
	}

	_CONSTEXPR20 bool NextColumn()
	{
		ImGuiContext& g = *GImGui;
		ImGuiTable* table = g.CurrentTable;
		return SetColumnIndex(table->CurrentColumn + 1);
	}

	_CONSTEXPR20 bool SetColumnIndex(int index)
	{
		return ImGui::TableSetColumnIndex(index);
	}

	// nMaxItem: enforce the row/item limit
	// bypass the Execution::Continue returned from the RenderCallback
	void Render(size_t nMaxItem, ImVec2 Size = { 0.f, 0.f })
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

}