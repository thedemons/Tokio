#pragma once
#include "Widgets.hpp"
#include "WPopup.h"

#include "Common/TypeDef.h"

namespace Widgets
{

// Tree table widget
// Copied from the Widgets::Table with some reworks
template <typename UserNode>
class TreeTable
{
public:
	
	// Make your tree data as UserNode and inherit this class
	// Then pass an std::vector<UserNode>& to ::Render()
	class NestedNode
	{
		template<typename> friend class TreeTable;
	private:
		size_t m_totalChilds = 0;		// total number of this child
		size_t m_level       = 0;       // the depth level from the root

		UserNode* m_parent   = nullptr;	// parent of this node
		std::vector<UserNode> m_childs;	// childrens of this node

	public:
		bool m_open = false;			// collapsed by default
		bool m_NoPushOnOpen = false;	// set to true if you use ImGuiTreeNodeFlags_NoTreePushOnOpen for this node
										// this is not so clean, i might rework it in the future

		// empty constructor
		NestedNode() {}

		// construct a child
		NestedNode(UserNode* lpParent) :
			m_parent(lpParent), m_level(lpParent->m_level + 1)
		{
			lpParent->m_totalChilds++;
		}

		NestedNode(NestedNode* lpParent) : 
			m_parent(reinterpret_cast<UserNode*>(lpParent)), m_level(lpParent->m_level + 1) 
		{
			lpParent->m_totalChilds++;
		}

		// if this node has no childrens
		inline bool HasNoChilds() const
		{
			return m_childs.size() == 0;
		}

		inline UserNode& AddChild()
		{
			return m_childs.emplace_back(this);
		}

		_NODISCARD inline bool RemoveChild(UserNode* child)
		{
			for (auto it = m_childs.begin(); it != m_childs.end(); it++)
			{
				if (it._Ptr == child)
				{
					m_totalChilds--;
					m_childs.erase(it);
					return true;
				}
			}
			return false;
		}

		_NODISCARD inline bool RemoveChildAt(size_t index)
		{
			if (index >= m_childs.size()) return false;

			m_totalChilds--;
			m_childs.erase(m_childs.begin() + index);
			return true;
		}

		inline std::vector<UserNode>& Childs()
		{
			return m_childs;
		}
	};

	// return type for the Render Callback
	// if the callback never return Execution::Stop,
	// it will be enforced by the maximum number of items
	enum class Execution
	{
		Continue,	// continue rendering
		Stop,		// stop rendering
		Skip,		// skip this item
		SkipNode,	// skip this node, continue on the parent node
	};

	// LPRENDER_CALLBACK
	// Index is the index of the node, including nested childs
	// Level is the depth from the root node
	// 
	// index	node			level
	// 0		- dir1			0
	// 1		  . file 1		1
	// 2		  . file 2		1
	// 3		+ dir2			0		// 4 items in this dir are collapsed
	// 8		- dir3			0		// index jump to 8, because we skipped 4 items in the previous node [3, (4, 5, 6, 7), 8]
	// 9		  - dir4		1		
	// 10		    . file 3	2
	// 11			+ dir 5		2		// 3 items collapsed [11, (12, 13, 14), 15]
	// 15			. file 4	2
	typedef Execution(*LPRENDER_CALLBACK)     (TreeTable* table, UserNode& node, size_t index, size_t level, void* UserData);
	typedef void     (*LPINPUT_CALLBACK)      (TreeTable* table, UserNode* node, size_t index, void* UserData);
	typedef void     (*LPPOPUPRENDER_CALLBACK)(TreeTable* table, UserNode* node, size_t index, void* UserData);
	typedef void     (*LPSORT_CALLBACK)       (TreeTable* table, size_t column, ImGuiSortDirection direction, void* UserData);

	// Tree table setup description
	struct Desc
	{
		std::string Name;
		bool IsMultiSelection = false;
		ImGuiTableFlags Flags = ImGuiTableFlags_None;		// Flags for the table
		ImGuiTableFlags ExtraFlags = ImGuiTableFlags_None;	// Extra flag for row rendering
		TableFlags WidgetFlags = TableFlags::None;

		// specific font for row rendering, this won't be applied t0 the header row
		ImFont* RowFont = nullptr;

		// The amount of indent, it will use ImGuiStyle.IndentSpacing
		// by default if this is not set (<= 0.f)
		float IndentSpacing = 0.f;

		// The spacing between the actual starting x position
		// and the item being drawn, this spacing is to fit
		// the arrow '>' button to expand the tree
		float ItemOffset = 10.f;


		// You must handle the ordering of the selected
		// items by yourself, it won't be altered after the sort
		LPSORT_CALLBACK SortCallback = nullptr;

		// Called everyframe, after ImGui::EndTable
		// the index argument is the current hovered item
		// if it is -1 then no items is being hovered
		LPINPUT_CALLBACK InputCallback = nullptr;

		// MUST HAVE!! Callback to render your nodes
		// return	Execution::Continue to keep rendering
		//			Execution::Stop to stop rendering
		//			Execution::Skip to skip the node
		//			Execution::SkipNode to skip the node and continue rendering the parent node
		LPRENDER_CALLBACK RenderCallback = nullptr;

		// Use this if you want a popup openned when
		// the user right-clicked something
		// The index is of the item the user right-clicked
		// Render the popup in this callback
		LPPOPUPRENDER_CALLBACK PopupRenderCallback = nullptr;

		void* SortUserData        = nullptr;
		void* InputUserData       = nullptr;
		void* RenderUserData      = nullptr;
		void* PopupRenderUserData = nullptr;
	};

private:
	struct ColumnData
	{
		std::string Name;							              // unique name
		size_t Index                = 0;			              // auto increase
		float Width                 = 0.f;			              // initial width
		ImGuiTableColumnFlags Flags = ImGuiTableColumnFlags_None; // ImGui Flags
	};

	// table description
	Desc m_desc;

	// selected items
	std::vector<size_t> m_selected;

	// column data, add a column using ::AddColumn
	std::vector<ColumnData> m_columns;

	// UPTR_UNDEFINE means no index is hovered
	size_t m_hoveredIndex      = UPTR_UNDEFINED;

	// UPTR_UNDEFINED means no column is hovered
	size_t m_hoveredColumn     = UPTR_UNDEFINED;

	// Don't know if i should experiment using a "null" reference here
	UserNode* m_hoveredNode    = nullptr;

	// Cache the node & index on popup open
	size_t m_popupIndex        = UPTR_UNDEFINED;
	UserNode* m_popupNode      = nullptr;


	// current sort column index
	size_t m_currentSortColumn = 0;

	// current node list, this pointer is only valid when Render() begin
	// and become nullptr after the Render() ended
	std::vector<UserNode>* m_currentNodeList = nullptr;

	// current node offset, same as TreePush TreePop
	float m_nodeOffset = 0.f;

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
		m_desc.Name = GetUniqueName(m_desc.Name, this);

		if (m_desc.PopupRenderCallback != nullptr)
		{
			Popup::Desc popupDesc;
			popupDesc.RenderCallback = PopupRenderCallback;
			popupDesc.RenderUserData = this;
			m_popup.Setup(popupDesc);
		}

		if (m_desc.IndentSpacing <= 0.0f)
		{
			m_desc.IndentSpacing = ImGui::GetStyle().IndentSpacing;
		}
	}

	void AddColumn(const std::string& Name, ImGuiTableColumnFlags Flags = ImGuiTableColumnFlags_None, float width = 0.f)
	{
		ColumnData column;
		column.Name = Name;
		column.Width = width;
		column.Flags = Flags;
		column.Index = m_columns.size();

		m_columns.push_back(column);
	}

private:
	// return true if the row was clicked
	bool HandleRowInput(ImGuiTable* table, UserNode& node, size_t index)
	{
		// imgui_tables.cpp #2016
		// fix for one-column tables don't call ImGuiTable::EndCell to calculate the cell height
		auto* window = table->InnerWindow;
		float CurPosY2 = window->DC.CursorMaxPos.y + table->CellPaddingY;
		float RowPosY2 = (table->RowPosY2 < CurPosY2) ? CurPosY2 : table->RowPosY2;

		// get the row rect
		ImRect row_rect(
			table->WorkRect.Min.x,
			table->RowPosY1,
			table->WorkRect.Max.x,
			RowPosY2
		);
		row_rect.ClipWith(table->BgClipRect);

		if (row_rect.Min.y >= row_rect.Max.y) return false;

		bool bHover =
			ImGui::IsMouseHoveringRect(row_rect.Min, row_rect.Max, false) &&
			ImGui::IsWindowHovered(ImGuiHoveredFlags_None) &&
			!ImGui::IsAnyItemHovered();

		bool bLBtnDown = ImGui::IsMouseDown(0);
		bool bLBtnClicked = ImGui::IsMouseReleased(0);
		bool bItemSelected = IsItemSelected(index);

		ImU32 color = IM_COL32_DISABLE;

		if (bHover)
		{
			m_hoveredIndex = index;
			m_hoveredNode = &node;

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

		return bHover && bLBtnClicked;
	}

	void HandlePopup()
	{
		m_popup.Render();

		if (m_hoveredIndex == UPTR_UNDEFINED || m_hoveredNode == nullptr) return;

		// if the user right clicked a row
		if (ImGui::IsMouseReleased(1))
		{

			// the user clicked outside of any selected item
			if (!IsItemSelected(m_hoveredIndex))
			{
				// we clear the selected items, then set the current item to be selected
				ClearSelectedItems();
				AddSelectedItem(m_hoveredIndex);
			}

			// cache the index and node pointer
			m_popupIndex = m_hoveredIndex;
			m_popupNode = m_hoveredNode;


			// open the popup
			m_popup.Open(reinterpret_cast<void*>(m_hoveredIndex));
		}
	}

	static void PopupRenderCallback(Popup* popup, void* OpenUserData, void* UserData)
	{
		UNUSED(popup);
		UNUSED(OpenUserData);

		TreeTable<UserNode>* pThis = static_cast<TreeTable<UserNode>*>(UserData);

		// check if the cached node pointer is still at the same index
		// e.g. the node list has been changed, if so we will close
		// the popup to avoid any conflicts, this behavior might change
		// in the future
		
		UserNode* openedNode = pThis->GetNodeAtIndex(pThis->m_popupIndex);
		if (openedNode == nullptr || openedNode != pThis->m_popupNode)
		{
			pThis->m_popupIndex = UPTR_UNDEFINED;
			pThis->m_popupNode = nullptr;
			pThis->m_popup.Close();
			return;
		}

		// why don't we have a popup render callback?
		assert(pThis->m_desc.PopupRenderCallback != nullptr);

		// forward the callback
		// OpenUserData is the index hovered when the user right-clicked
		pThis->m_desc.PopupRenderCallback(pThis, pThis->m_popupNode, pThis->m_popupIndex, pThis->m_desc.PopupRenderUserData);
	}

	// Apply the indent spacing 
	inline void ApplyIndent(float customOffset = 0.f)
	{
		ImVec2 cursorPosition = ImGui::GetCursorPos();
		ImGui::SetCursorPos(cursorPosition + ImVec2(m_nodeOffset + m_desc.ItemOffset + customOffset, 0));
	}

	void RenderNode(ImGuiTable* table, UserNode& node, size_t& index, size_t level, Execution& state)
	{
		// if the last node was skipped, don't begin a new row
		// because we have began a new row for it and haven't closed it yet
		if (state != Execution::Skip && state != Execution::SkipNode)
		{
			ImGui::TableNextRow();
			SetColumnIndex(0);
		}

		state = m_desc.RenderCallback(this, node, index, level, m_desc.RenderUserData);

		// the node was skipped
		if (state != Execution::Continue)
		{
			// increase the index before skipping this node
			index += node.m_totalChilds + 1;
			return;
		}

		ImGui::TableSetColumnIndex(static_cast<int>(m_columns.size() - 1));

		// HandleRowInput() return true if the node was clicked
		if (HandleRowInput(table, node, index))
		{
			node.m_open = !node.m_open;
		}

		index += 1;
		level += 1;

		// if this node is open, we render its childs
		if (node.m_open && node.m_childs.size() > 0)
		{
			if (!node.m_NoPushOnOpen) TreePush();

			for (auto& child : node.m_childs)
			{
				RenderNode(table, child, index, level, state);
				if (state == Execution::Stop) return;
				if (state == Execution::SkipNode) break;
			}

			if (!node.m_NoPushOnOpen)
			{
				TreePop();

				// IMPORTANT: Fix a bug in logic
				// If the last node is skipped, it won't make begin a new row
				// and apply indent, so the indent stuck at whatever the last
				// node's indent was
				if (state == Execution::Skip || state == Execution::SkipNode)
					ApplyIndent(-m_desc.ItemOffset - m_desc.IndentSpacing);
			}
		}
		// if it isn't open, we add the child size the index
		else
		{
			index += node.m_totalChilds;
		}
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

	_NODISCARD _CONSTEXPR20 std::vector<size_t>& GetSelectedItems()
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

	_NODISCARD _CONSTEXPR20 size_t GetHoveredColumn() const
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

	// Push the tree by one indent
	_CONSTEXPR20 void TreePush()
	{
		m_nodeOffset += m_desc.IndentSpacing;
	}

	// Pop the tree by one indent
	_CONSTEXPR20 void TreePop()
	{
		m_nodeOffset -= m_desc.IndentSpacing;
	}

	// Get the item x offset
	_NODISCARD _CONSTEXPR20 float GetItemOffset()
	{
		return m_nodeOffset + m_desc.ItemOffset;
	}

	// Use this instead of ImGui::TableNextColumn()
	// to handle the indent cursor position automatically
	_CONSTEXPR20 bool NextColumn()
	{
		ImGuiContext& g = *GImGui;
		ImGuiTable* table = g.CurrentTable;
		return SetColumnIndex(table->CurrentColumn + 1);
	}

	_CONSTEXPR20 bool SetColumnIndex(int index)
	{
		bool result = ImGui::TableSetColumnIndex(index);
		ApplyIndent();
		return result;
	}

	_NODISCARD _CONSTEXPR20 UserNode* GetNodeAtIndex(size_t index)
	{
		if (m_currentNodeList == nullptr) return nullptr;

		static const auto GetNodeRecursive =
		[](auto&& GetNodeRecursive, UserNode* node, size_t& recursiveIndex, size_t indexToFind) -> UserNode*
		{
			if (recursiveIndex++ == indexToFind) return node;
			for (auto& child : node->m_childs)
			{
				UserNode* result = GetNodeRecursive(GetNodeRecursive, &child, recursiveIndex, indexToFind);
				if (result != nullptr) return result;
			}
			return nullptr;
		};

		size_t recursiveIndex = 0;
		for (auto it = m_currentNodeList->begin(); it != m_currentNodeList->end(); it++)
		{
			auto result = GetNodeRecursive(GetNodeRecursive, it._Ptr, recursiveIndex, index);
			if (result != nullptr) return result;
		}
		return nullptr;
	}


	// The widget will modify the node m_open state only, userdata will be left untouched
	void Render(std::vector<UserNode>& NodeList, ImVec2 Size = { 0.f, 0.f })
	{
		m_currentNodeList = &NodeList;

		ImGuiContext& g = *GImGui;

		int columnCount = static_cast<int>(m_columns.size());

		if (ImGui::BeginTable(m_desc.Name.c_str(), columnCount, m_desc.Flags, Size))
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
						static_cast<size_t>(sorts_specs->Specs->ColumnIndex),
						sorts_specs->Specs->SortDirection
					);
				}
			}

			// reset hover index before we rendering any rows
			// notice that the m_hoveredIndex of the last
			// frame will still be available for SortCallback
			// to use, we might use it in the future
			m_hoveredIndex = UPTR_UNDEFINED;
			m_hoveredNode = nullptr;

			// reset node offset before we begin
			m_nodeOffset = 0.f;

			if (m_desc.RowFont != nullptr) ImGui::PushFont(m_desc.RowFont);

			size_t index = 0;
			Execution state = Execution::Continue;
			for (auto& node : NodeList)
			{
				RenderNode(table, node, index, 0, state);
				if (state == Execution::Stop) break;
			}

			if (m_desc.RowFont != nullptr) ImGui::PopFont();

			// it means that we begin a table row
			// and never used it, if we don't "destroy" it
			// it will glitched into the table as a tiny teeny row
			// i don't know if it is appropriate to do this
			// this is EXTREMELY hacky and dirty, changing the internal
			// of imgui, but it works fine so be it!
			if (state == Execution::Skip || state == Execution::SkipNode)
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
				m_desc.InputCallback(this, m_hoveredNode, m_hoveredIndex, m_desc.InputUserData);

			// render the popup if we have one
			if (m_desc.PopupRenderCallback) HandlePopup();
		}

		// WARNING: For a reason, we store the pointer to the last frame nodes
		//			This might, at some point, crash because the pointer is no longer valid
		// m_currentNodeList = nullptr;
	}

};

}