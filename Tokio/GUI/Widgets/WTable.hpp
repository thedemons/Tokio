#pragma once
#include "WPopup.hpp"

namespace Widgets
{

class TableData
{
public:
	std::vector<int> actives;
	int nCol = 0;
	int maxItem = 0;
	int doubleClicked = -1;
	int rightClicked = -1;
	int clicked = -1;

	void reset()
	{
		actives.clear();
		maxItem = 0;
	}

	void active_item(int i)
	{
		bool bActive = std::find(actives.begin(), actives.end(), i) != actives.end();
		clicked = i;

		bool bCtrlDown = ImGui::IsKeyDown(ImGuiKey_::ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_::ImGuiKey_RightCtrl);
		bool bShiftDown = ImGui::IsKeyDown(ImGuiKey_::ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_::ImGuiKey_RightShift);

		if (bShiftDown && actives.size() > 0)
		{
			int startIndex = actives.back();

			if (!bCtrlDown)
			{
				actives.clear();
				actives.push_back(startIndex);
			}

			if (startIndex != i)
			{
				if (startIndex > i)
				{
					for (int index = startIndex - 1; index >= i; index--)
					{
						if (std::find(actives.begin(), actives.end(), index) == actives.end())
							actives.push_back(index);
					}
				}
				else
				{
					for (int index = startIndex + 1; index <= i; index++)
					{
						if (std::find(actives.begin(), actives.end(), index) == actives.end())
							actives.push_back(index);
					}
				}
			}
		}
		else if (bCtrlDown)
		{
			if (!bActive)
				actives.push_back(i);
			else if (!bShiftDown)
				actives.erase(std::find(actives.begin(), actives.end(), i));
		}
		else if (ImGui::IsMouseReleased(1))
		{
			if (std::find(actives.begin(), actives.end(), i) == actives.end())
			{
				actives.clear();
				actives.push_back(i);
			}
		}
		else
		{
			actives.clear();
			actives.push_back(i);
		}
	}
};


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

	typedef void (*LPINPUT_CALLBACK)(Table* table, INT index, void* UserData);
	typedef Execution (*LPRENDER_CALLBACK)(Table* table, UINT index, void* UserData);
	typedef void (*LPPOPUPRENDER_CALLBACK)(Table* table, UINT index, void* UserData);
	typedef void (*LPSORT_CALLBACK)(Table* table, UINT column, ImGuiSortDirection direction, void* UserData);

	// input callback, this will be call in the same frame
	// so just use functions like ImGui::IsMouseDown to 
	// query the mouse and keyboard input, the only thing that
	// the table tell you is which row are being hovered on

	struct Desc
	{
		std::string Name;
		bool IsMultiSelection = false;
		ImGuiTableFlags Flags = ImGuiTableFlags_None;

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

public:
	struct ColumnData
	{
		UINT Index = 0;			// auto increase
		float Width = 0.f;		// initial width
		std::string Name;		// unique name
		ImGuiTableColumnFlags Flags = ImGuiTableColumnFlags_None;
	};

	// table description
	Desc m_desc;

	// selected items
	std::vector<UINT> m_selected;

	// column data, add a column using ::AddColumn
	std::vector<ColumnData> m_columns;

	// -1 means no index is hovered
	INT m_currentHoverIndex = 0;

	// current sort column index
	UINT m_currentSortColumn = 0;

	// current sort direction
	ImGuiSortDirection m_currentSortDir = ImGuiSortDirection_Ascending;

	// popup widget for right click handling
	Popup m_popup;

	TableData data;


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
		column.Index = static_cast<UINT>(m_columns.size());

		m_columns.push_back(column);
	}

private:
	void HandleRowInput(ImGuiTable* table, UINT index)
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

		bool bHover = ImGui::IsMouseHoveringRect(row_rect.Min, row_rect.Max, false) &&
			ImGui::IsWindowFocused(ImGuiFocusedFlags_None);

		if (bHover) m_currentHoverIndex = index;

		if (IsItemSelected(index))
		{
			table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
			//table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_ButtonActive);

		}
		else if (bHover)
		{
			bool bLBtnDown    =	ImGui::IsMouseDown(0);
			bool bLBtnClicked = ImGui::IsMouseReleased(0);

			ImU32 colorHover = ImGui::GetColorU32(ImGuiCol_Button);
			if (bLBtnDown) colorHover = ImGui::GetColorU32(ImGuiCol_ButtonHovered);

			// override row bg color (imgui_tables.cpp line 1771)
			table->RowBgColor[1] = colorHover;

			if (bLBtnClicked) AddSelectedItem(index);
		}
	}

	void HandlePopup()
	{
		if (m_desc.PopupRenderCallback == nullptr) return;

		m_popup.Render();

		if (m_currentHoverIndex < 0) return;

		// if the user right clicked a row
		if (ImGui::IsMouseReleased(1))
		{
			UINT index = static_cast<UINT>(m_currentHoverIndex);

			// the user clicked outside of any selected item
			if (!IsItemSelected(index))
			{
				// we clear the selected items, then set the current item to be selected
				ClearSelectedItems();
				AddSelectedItem(index);
			}

			// open the popup
			m_popup.Open(reinterpret_cast<void*>(UINT_PTR(index)));
		}

	}

	static void PopupRenderCallback(Popup* popup, void* OpenUserData, void* UserData)
	{
		Table* pThis = static_cast<Table*>(UserData);

		// why don't we have a popup render callback?
		assert(pThis->m_desc.PopupRenderCallback != nullptr);

		// forward the callback
		// OpenUserData is the index hovered when the user right-clicked
		UINT index = SIZE_T(OpenUserData) & 0xFFFFFFFF;
		pThis->m_desc.PopupRenderCallback(pThis, index, pThis->m_desc.PopupRenderUserData);
	}

public:
	void AddSelectedItem(UINT index)
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

	_CONSTEXPR20 std::vector<UINT>& GetSelectedItems()
	{
		return m_selected;
	}

	_CONSTEXPR20 bool IsItemSelected(UINT index) const
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

	// Get the popup to interact with it
	// becareful not to break anything
	constexpr Popup& GetPopup()
	{
		return m_popup;
	}

	// Manually trigger the sort callback
	// User must specify the m_desc.SortCallback beforehand
	constexpr void Sort()
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
	constexpr void Sort(UINT column, ImGuiSortDirection direction)
	{
		if (!m_desc.SortCallback) return;

		m_currentSortColumn = column;
		m_currentSortDir = direction;

		return Sort();
	}

	// nMaxItem: enforce the row/item limit
	// bypass the Execution::Continue returned from the RenderCallback
	void Render(UINT nMaxItem, ImVec2 Size = { 0.f, 0.f })
	{
		ImGuiContext& g = *GImGui;

		if (ImGui::BeginTable(m_desc.Name.c_str(), static_cast<UINT>(m_columns.size()), m_desc.Flags, Size))
		{
			auto* table = g.CurrentTable;

			// Make top row always visibles
			ImGui::TableSetupScrollFreeze(0, 1);


			for (auto& column : m_columns)
			{
				ImGui::TableSetupColumn(column.Name.c_str(), column.Flags, column.Width);
			}

			// weird imgui stuff, why not do this automatically?
			ImGui::TableHeadersRow();

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
			m_currentHoverIndex = -1;

			Execution state = Execution::Continue;

			for (UINT i = 0; state != Execution::Stop && i < nMaxItem; i++)
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

			ImGui::EndTable();

			// call input callback
			if (m_desc.InputCallback)
				m_desc.InputCallback(this, m_currentHoverIndex, m_desc.InputUserData);

			HandlePopup();
		}
	}

	void Render2()
	{
		data.doubleClicked = -1;
		data.rightClicked = -1;
		data.clicked = -1;


		float scroll_y = ImGui::GetScrollY();
		ImVec2 winPos = ImGui::GetWindowPos();
		ImVec2 winSize = ImGui::GetWindowSize();

		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 itemSize;
		itemSize.y = style.ItemSpacing.y * 2.f + ImGui::GetTextLineHeight();
		itemSize.x = 3;

		for (int i = 0; i < (data.nCol == 0 ? ImGui::TableGetColumnCount() : data.nCol); i++)
		{
			ImGui::TableSetColumnIndex(i);
			itemSize.x += ImGui::GetColumnWidth() + style.CellPadding.x * 2;
		}
		ImGui::TableSetColumnIndex(0);

		ImVec2 cursorPos = ImGui::GetCursorPos();

		cursorPos.x += winPos.x - style.CellPadding.x;
		cursorPos.y = winPos.y - scroll_y - style.CellPadding.y;

		bool bWindowHover = ImGui::IsWindowHovered();

		int i_real = 0;
		int doubleClickIndex = -1;


		ImDrawList* pDraw = ImGui::GetWindowDrawList();
		ImDrawList* pDraw2 = nullptr;

		if (ImGui::GetCurrentWindow()->ParentWindow)
			pDraw2 = ImGui::GetCurrentWindow()->ParentWindow->DrawList;

		//ImGui::GetForegroundDrawList()->AddRect(winPos, winPos + winSize, 0xff00ff00);

		bool bHoverAny = false;
		for (int i = 0; i < data.maxItem; i++)
		{

			ImVec2 rect_min = { cursorPos.x, cursorPos.y + itemSize.y * (i + 1) };
			ImVec2 rect_max = { cursorPos.x + itemSize.x, cursorPos.y + itemSize.y * (i + 2) };

			// not drawing for performance
			if ((rect_min.y < winPos.y) || (rect_min.y > winPos.y + winSize.y))
			{
				continue;
			}

			//char debug[255];
			//sprintf_s(debug, "%d", i);
			//ImGui::GetForegroundDrawList()->AddText(rect_min, ThemeColor.green, debug);
			//
			bool bActive = std::find(data.actives.begin(), data.actives.end(), i) != data.actives.end();

			bool bHover = ImGui::IsMouseHoveringRect(rect_min, rect_max, false) && ImGui::IsWindowHovered();
			bHoverAny |= bHover;

			ImGui::GetForegroundDrawList()->AddRect(rect_min, rect_max, 0xff00ff00);

			if (bHover || bActive)
			{
				ImU32 color;

				if (bActive) color = ImGui::GetColorU32(ImGuiCol_::ImGuiCol_ButtonActive);
				if (bHover)
				{
					color = ImGui::GetColorU32(ImGuiCol_::ImGuiCol_ButtonHovered);

					if (ImGui::IsMouseDown(0)) color = ImGui::GetColorU32(ImGuiCol_::ImGuiCol_ButtonActive);

					if (ImGui::IsMouseDoubleClicked(0))
						data.doubleClicked = i;

					bool isClicked = ImGui::IsMouseReleased(0);
					bool isRClicked = ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Right);

					if (isRClicked) data.rightClicked = i;

					if (isClicked || isRClicked)
					{
						data.active_item(i);
					}
				}

				pDraw->AddRectFilled(rect_min, rect_max, color);
				if (pDraw2) pDraw2->AddRectFilled(rect_min, rect_max, color);
			}
		}

		if (!bHoverAny && ImGui::IsWindowHovered() && ImGui::IsMouseReleased(0))
		{
			data.actives.clear();
		}
	}
};

}