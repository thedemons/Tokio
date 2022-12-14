/*
 * Project Tokio
 * Author: thedemons
 * Github: thedemons/Tokio
 * Made:   With love
 *
 * License: MIT License
 *
 * Copyright(c) 2022 thedemons
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#ifndef TOKIO_GUI_WIDGETS_WTABLE_H
#define TOKIO_GUI_WIDGETS_WTABLE_H

#include <string>
#include <vector>

#include "imgui.hpp"

#include "WPopup.h"
#include "WidgetsFlags.hpp"
#include "Common/TypeDef.h"

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

		// specific font for row rendering, this won't be applied to the header row
		ImFont* RowFont = nullptr;

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


	ImGuiTable* m_currentTable = nullptr;


	// use by SetScroll()
	float m_setScrollY = -1.f;


public:
	void Setup(const Desc& desc);

	void AddColumn(const std::string& Name, ImGuiTableColumnFlags Flags = ImGuiTableColumnFlags_None, float width = 0.f);

private:
	void HandleRowInput(ImGuiTable* table, size_t index);

	void HandlePopup();

	static void PopupRenderCallback(Popup* popup, void* OpenUserData, void* UserData);

public:
	void AddSelectedItem(size_t index);


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

	_CONSTEXPR20 bool NextColumn() const
	{
		return SetColumnIndex(m_currentTable->CurrentColumn + 1);
	}

	_CONSTEXPR20 bool SetColumnIndex(int index) const
	{
		return ImGui::TableSetColumnIndex(index);
	}

	_CONSTEXPR20 void SetScroll(float Y)
	{
		m_setScrollY = Y;
	}

	_CONSTEXPR20 size_t GetHoveredIndex() const
	{
		return m_hoveredIndex;
	}

	_NODISCARD _CONSTEXPR20 ImGuiTable* GetHandle() const
	{
		return m_currentTable;
	}
	

	// nMaxItem: enforce the row/item limit
	// bypass the Execution::Continue returned from the RenderCallback
	void Render(size_t nMaxItem, ImVec2 Size = { 0.f, 0.f });

};

}
#endif // !TOKIO_GUI_WIDGETS_WTABLE_H
