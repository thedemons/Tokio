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

// Thanks to the author of this awesome library, it helped me alot with getting file icon
// https://github.com/dfranx/ImFileDialog/blob/main/ImFileDialog.h

#pragma once
#include "stdafx.h"
#include "ViewAttachProc.h"

#include "Graphics.h"
#include "GUI/Widgets/Widgets.hpp"

#include "Engine/Engine.h"

#include "Common/Exception.h"
#include "Common/StringHelper.h"
#include "Common/PathHelper.h"
#include "Common/SystemHelper.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>

namespace ViewAttachProcUtils
{

std::map<HICON, ImTextureID> m_iconIndices;
std::map<WindowHandle, ImTextureID> m_windowIcons;
std::map<std::wstring, ImTextureID> m_fileIcons;



HICON GetFileHIcon(const std::wstring& path) NULLABLE
{

	DWORD attrs = 0;
	std::error_code ec;
	UINT flags = SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES;

	if (!std::filesystem::exists(path, ec))
	{
		attrs = FILE_ATTRIBUTE_DIRECTORY;
	}

	SHFILEINFOW fileInfo = { 0 };
	SHGetFileInfoW(path.c_str(), attrs, &fileInfo, sizeof(SHFILEINFOW), flags);

	return fileInfo.hIcon;
}

ImTextureID GetTextureFromHIcon(const HICON hIcon) NULLABLE
{
	// check if hFileIcon is already loaded
	auto findIcon = m_iconIndices.find(hIcon);
	if (findIcon != m_iconIndices.end()) return findIcon->second;

	// Get the icon info 
	ICONINFO iconInfo = { 0 };
	if (!GetIconInfo(hIcon, &iconInfo)) return nullptr;

	// get bitmap info
	DIBSECTION ds;
	if (!GetObjectW(iconInfo.hbmColor, sizeof(ds), &ds)) return nullptr;

	UINT width = ds.dsBm.bmWidth;
	UINT height = ds.dsBm.bmHeight;

	// calculate the buffer size needed
	// ds.dsBm.bmBitsPixel is the number of bit per pixel
	LONG byteSize = width * height * (ds.dsBm.bmBitsPixel / 8);

	// copy bitmap to the buffer
	std::vector<byte_t> bitmapBuffer(static_cast<size_t>(byteSize));
	LONG resultGetBm = GetBitmapBits(iconInfo.hbmColor, byteSize, bitmapBuffer.data());
	if (resultGetBm == 0) return nullptr;


	// create the texture
	auto texture = Graphics::TextureFromBitmap(bitmapBuffer.data(), width, height);
	if (texture == nullptr) return nullptr;

	// cache the icon
	m_iconIndices[hIcon] = texture;

	return texture;
}

ImTextureID GetFileIconTexture(const std::wstring& path) NULLABLE
{
	// return the stored texture if we have loaded it already
	auto findIcon = m_fileIcons.find(path);
	if (findIcon != m_fileIcons.end()) return findIcon->second;

	HICON hFileIcon = GetFileHIcon(path);
	if (hFileIcon == nullptr) return nullptr;

	auto texture = GetTextureFromHIcon(hFileIcon);
	if (texture == nullptr) return nullptr;

	m_fileIcons[path] = texture;

	return texture;
}

ImTextureID GetWindowIconTexture(WindowHandle hwnd) NULLABLE
{
	// return the stored texture if we have loaded it already
	auto findIcon = m_windowIcons.find(hwnd);
	if (findIcon != m_windowIcons.end()) return findIcon->second;

	HICON hFileIcon = reinterpret_cast<HICON>(GetClassLongPtrW(static_cast<HWND>(hwnd), -14));
	if (hFileIcon == nullptr) return nullptr;

	auto texture = GetTextureFromHIcon(hFileIcon);
	if (texture == nullptr) return nullptr;

	m_windowIcons[hwnd] = texture;

	return texture;
}


}

// Table Callback
Widgets::Table::Execution
ViewAttachProc::TableRenderCallback(Widgets::Table* table, size_t index, void* UserData)
{
	UNUSED(table);

	ViewAttachProc* pThis = static_cast<ViewAttachProc*>(UserData);
	ProcessListData& procData = pThis->m_processList[index];

	if (procData.isHidden) return Widgets::Table::Execution::Skip;

	// icon
	if (procData.icon) ImGui::Image(procData.icon, { 15, 15 });

	// process
	ImGui::TableNextColumn();
	ImGui::Text("%s", procData.name.c_str());

	// window
	ImGui::TableNextColumn();
	if (procData.windows.size() > 0)
	{
		auto& firstWindow = procData.windows[0];
		if (procData.windows.size() == 1)
		{
			ImGui::TreeNodeEx(firstWindow.hidden_title.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);

			ImGui::SameLine();
			if (firstWindow.icon)
			{
				ImGui::Image(firstWindow.icon, { 15, 15 });
				ImGui::SameLine();
			}
			ImGui::Text("%s", firstWindow.title.c_str());
		}
		else
		{

			// make the tree node look better on hover
			ImGuiContext& g = *GImGui;
			float oldPaddingX = g.CurrentTable->CellPaddingX;
			float oldPaddingY = g.CurrentTable->CellPaddingY;

			g.CurrentTable->CellPaddingX = 0.f;
			g.CurrentTable->CellPaddingY = 0.f;

			// rendering image in tree node
			// https://github.com/ocornut/imgui/issues/282
			bool bTreeOpen = ImGui::TreeNodeEx(firstWindow.hidden_title.c_str(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);

			g.CurrentTable->CellPaddingX = oldPaddingX;
			g.CurrentTable->CellPaddingY = oldPaddingY;

			ImGui::SameLine();

			if (firstWindow.icon)
			{
				ImGui::Image(firstWindow.icon, { 15, 15 });
				ImGui::SameLine();
			}

			ImGui::Text("%s", firstWindow.title.c_str());
			if (bTreeOpen)
			{
				UINT skip_first = 0;
				for (auto& window : procData.windows)
				{
					if (skip_first++ == 0) continue;

					if (window.icon)
					{
						ImGui::Image(window.icon, { 15, 15 });
						ImGui::SameLine();
					}

					ImGui::Text("%s", window.title.c_str());
				}
				ImGui::TreePop();
			}
		}

	}

	// pid
	ImGui::TableNextColumn();
	ImGui::Text("%-6d | 0x%x", procData.pid, procData.pid);

	// file location
	ImGui::TableNextColumn();
	ImGui::Text("%s", procData.path.c_str());
	return Widgets::Table::Execution::Continue;
};

void ViewAttachProc::TableSortCallback(Widgets::Table* table, size_t column, ImGuiSortDirection direction, void* UserData)
{
	UNUSED(table);

	ViewAttachProc* pThis = static_cast<ViewAttachProc*>(UserData);
	bool isAscending = direction == ImGuiSortDirection_Ascending;

	DWORD oldSelectedPID = 0;
	if (auto& oldSelectedItems = pThis->m_table.GetSelectedItems(); oldSelectedItems.size() > 0)
	{
		oldSelectedPID = pThis->m_processList[oldSelectedItems[0]].pid;
	}

	// using multiple different function instead of
	// using switch(case) in one function helps a lot with the performance
	// not really crucial here but i don't like slow things..

	// sort by the process' creation time
	auto sortByTime = [=](const ProcessListData& a, const ProcessListData& b) -> bool
	{
		return isAscending ?
			a.creationTime < b.creationTime :
			a.creationTime > b.creationTime;
	};

	// sort by process' name
	auto sortByProcess = [=](const ProcessListData& a, const ProcessListData& b) -> bool
	{
		int cmp = std::strcmp(a.name.c_str(), b.name.c_str());
		if (cmp == 0) return sortByTime(a, b);

		return isAscending ? cmp < 0 : cmp > 0;
	};

	// TODO: Implement per-window table
	// sort by process' window title
	auto sortByWindow = [=](const ProcessListData& a, const ProcessListData& b) -> bool
	{
		bool aHasWindow = a.windows.size() > 0;
		bool bHasWindow = b.windows.size() > 0;

		// if both don't have window(s)
		if (!aHasWindow && !bHasWindow) return sortByTime(a, b);

		if (!aHasWindow && bHasWindow) return isAscending;
		if (aHasWindow && !bHasWindow) return !isAscending;

		// both have window(s)

		int cmp = std::strcmp(a.windows[0].title.c_str(), b.windows[0].title.c_str());
		if (cmp == 0) return sortByTime(a, b);

		return isAscending ? cmp < 0 : cmp > 0;
	};

	// sort by process' pid
	auto sortByPID = [=](const ProcessListData& a, const ProcessListData& b) -> bool
	{
		return isAscending ? a.pid < b.pid : a.pid > b.pid;
	};

	// i know i know, but making a switch statement here would
	// make the code super duper long and i don't think i'd like that
	std::function sortFunction = sortByTime;
	if (column == 1) sortFunction = sortByProcess;
	else if (column == 2) sortFunction = sortByWindow;
	else if (column == 3) sortFunction = sortByPID;

	std::sort(pThis->m_processList.begin(), pThis->m_processList.end(), sortFunction);

	// restore the selected process
	if (oldSelectedPID != 0) pThis->SelectProcessByPid(oldSelectedPID);
}

void ViewAttachProc::TableInputCallback(Widgets::Table* table, size_t index, void* UserData)
{
	UNUSED(table);

	ViewAttachProc* pThis = static_cast<ViewAttachProc*>(UserData);

	// if any row is hovered
	if (index != UPTR_UNDEFINED)
	{
		if (ImGui::IsMouseDoubleClicked(0))
		{
			DWORD pid = pThis->m_processList[index].pid;
			try
			{
				Engine::Attach(pid);
				ImGui::CloseCurrentPopup();
			}
			catch (Tokio::Exception& e)
			{
				e.Log("Couldn't attach to the process");
			}

		}
	}
}

void ViewAttachProc::TablePopupRenderCallback(Widgets::Table* table, size_t index, void* UserData)
{
	// popup events are guaranteed to have at least one selected item
	index = table->GetSelectedItems()[0];

	ViewAttachProc* pThis = static_cast<ViewAttachProc*>(UserData);
	ProcessListData& procData = pThis->m_processList[index];

	if (procData.icon != nullptr)
	{
		ImGui::Image(procData.icon, { 15, 15 });
		ImGui::SameLine();
	}

	ImGui::Text("%s", procData.name.c_str());
	ImGui::Separator();

	if (ImGui::Selectable(ICON_FILEFOLDER u8" Open file location"))
	{
		if (!Tokio::OpenFileInExplorer(procData.wpath))
		{
			Tokio::Log("Could not open file in explorer: %s", procData.path.c_str());
		}
	}

	if (ImGui::Selectable(ICON_KILL u8" Kill process"))
	{

	}

	if (ImGui::Selectable(ICON_SUSPEND u8" Suspend process"))
	{

	}
}

void ViewAttachProc::FilterEditCallback(Widgets::TextInput* tinput, ImGuiInputTextCallbackData* data, void* UserData)
{
	UNUSED(tinput);
	ViewAttachProc* pThis = static_cast<ViewAttachProc*>(UserData);

	// the filter contains nothing
	if (data->BufTextLen == 0)
	{
		for (auto& procData : pThis->m_processList)
			procData.isHidden = false;
	}
	else
	{
		std::string filter = std::string(data->Buf, static_cast<size_t>(data->BufTextLen));
		filter = Tokio::StringLower(filter);

		for (auto& procData : pThis->m_processList)
			procData.isHidden = pThis->IsItemFiltered(procData, filter);
	}
}

ViewAttachProc::ViewAttachProc()
{
	m_title = ICON_ATTACH_PROC u8" Attach Process";

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

	desc.Flags = ImGuiTableFlags_RowBg | // two background mode
		ImGuiTableFlags_Borders | // row borders
		ImGuiTableFlags_ScrollY | // enable vertical scrool
		ImGuiTableFlags_Hideable | // hide the columns
		ImGuiTableFlags_Sortable | // enable sorting
		ImGuiTableFlags_Resizable | // not sure what this does
		ImGuiTableFlags_Reorderable; // re-order the coulmns

	m_table.Setup(desc);

	// flags for the first column, aka the icon/creation time
	auto colFlags = ImGuiTableColumnFlags_NoHide | // don't hide me please
		ImGuiTableColumnFlags_NoResize | // do not resize me also
		ImGuiTableColumnFlags_WidthFixed | // need this to initialize the width
		ImGuiTableColumnFlags_DefaultSort | // default sort by creation time
		ImGuiTableColumnFlags_PreferSortDescending; // latest spawned process will be on top

	m_table.AddColumn("", colFlags, 15.f);
	m_table.AddColumn("Process");
	m_table.AddColumn("Window");
	m_table.AddColumn("PID");
	m_table.AddColumn("File Location", ImGuiTableColumnFlags_DefaultHide);


	// setup the filter text input
	Widgets::TextInput::Desc tiDesc;
	tiDesc.Label = "##Filter";
	tiDesc.Hint = "Search for a process, window or pid";
	tiDesc.EditCallback = FilterEditCallback;
	tiDesc.EditUserData = this;

	m_textFilter.Setup(tiDesc);

}


void ViewAttachProc::Render(bool& bOpen)
{
	if (m_isOpenned != bOpen)
	{
		m_isOpenned = bOpen;
		if (m_isOpenned)
		{
			GetProcessList();
			ImGui::OpenPopup(Title().c_str());
		}
	}

	if (!m_isOpenned) return;

	// check for refresh interval
	double timeSinceLastRefresh = ImGui::GetTime() - m_timeLastRefresh;
	if (timeSinceLastRefresh > m_refreshInterval)
		GetProcessList();

	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 0,0 });

	if (ImGui::BeginPopupModal(Title().c_str(), &m_isOpenned))
	{
		ImGui::PopStyleVar();

		ImGui::SetNextItemWidth(-1);
		m_textFilter.Render();

		m_table.Render(m_processList.size());

		ImGui::EndPopup();
	}
	else
	{
		bOpen = false;
		ImGui::PopStyleVar();
	}
}

bool ViewAttachProc::IsItemFiltered(const ProcessListData& data, const std::string& filter)
{
	// if the filter is empty then it's pass
	//if (filter.length() == 0 || filter.data()[0] == 0) return false;

	// return true if the text passed the filter
	const auto checkStr = [=](const std::string& text) -> bool
	{
		std::string textLower = Tokio::StringLower(text);
		auto find = textLower.find(filter);
		return find != std::string::npos;
	};

	// check the process name
	if (checkStr(data.name)) return false;

	// check the process' windows
	for (auto& window : data.windows)
	{
		if (checkStr(window.title)) return false;
		if (checkStr(window.classname)) return false;
	}


	// TODO: FIX THIS
	char parseString[64];
	sprintf_s(parseString, "%d 0x%x", data.pid, data.pid);
	if (checkStr(parseString)) return false;

	return true;
}

void ViewAttachProc::SelectProcessByPid(PID pid)
{
	for (auto& procData : m_processList)
	{
		if (procData.pid == pid)
		{
			size_t index = static_cast<size_t>(&procData - &m_processList[0]);
			m_table.AddSelectedItem(index);
			break;
		}
	}
}

void ViewAttachProc::GetProcessList()
{
	std::map<PID, ProcessEntry> procList;
	try
	{
		procList = Tokio::GetAllProcess();
	}
	catch (Tokio::Exception& e)
	{
		e.Log("Could not get the process list");
		return;
	}


	// remember the current selected process in the table
	PID oldSelectedPID = 0;
	if (auto& oldSelectedItems = m_table.GetSelectedItems(); oldSelectedItems.size() > 0)
	{
		oldSelectedPID = m_processList[oldSelectedItems[0]].pid;
	}

	// clear the current list first
	m_processList.clear();
	m_table.ClearSelectedItems();

	// push these processes into m_processList
	for (auto& [pid, entry] : procList)
	{

		// check if we have a cached data for this pid
		if (auto findProc = m_processCache.find(pid); findProc != m_processCache.end())
		{
			m_processList.push_back(findProc->second);
			continue;
		}

		ProcessListData& procData = m_processList.emplace_back();
		procData.pid = pid;
		procData.entry = entry;
		procData.wname = entry.szExe;
		procData.name = Tokio::String(procData.wname);
		procData.creationTime = 0;

		// get the full file path of process
		if (HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, pid))
		{
			// get full file path
			{
				DWORD sizeRequired = 1024;
				procData.wpath.resize(sizeRequired);

				QueryFullProcessImageNameW(hProc, 0, procData.wpath.data(), &sizeRequired);

				procData.wpath.resize(sizeRequired);

				// get the file icon
				if (sizeRequired > 0)
				{
					procData.path = Tokio::String(procData.wpath);
					procData.icon = ViewAttachProcUtils::GetFileIconTexture(procData.wpath);
				}
			}

			// get creation time
			{
				FILETIME creationTime, exitTime, kernelTime, userTime;
				if (GetProcessTimes(hProc, &creationTime, &exitTime, &kernelTime, &userTime))
				{
					procData.creationTime = *(uint64_t*)&creationTime;
				}
			}

		}

		// cache the process data
		m_processCache[procData.pid] = procData;
	}

	// find their associated hwnd
	try
	{
		auto windowList = Tokio::GetAllWindows();
		for (auto& procData : m_processList)
		{

			for (auto& winData : windowList)
			{
				if (winData.pid != procData.pid) continue;
				if (winData.title.size() == 0) continue;

				// FIXME: MIGHT WANNA DISABLE THIS!!
				// skip system's non-relavant windows
				if (winData.title == L"MSCTFIME UI" || winData.title == L"Default IME") continue;

				procData.windows.push_back(WindowData());
				WindowData& window = procData.windows.back();

				window.pid = procData.pid;
				window.hwnd = winData.hwnd;
				window.title = Tokio::String(winData.title);
				window.classname = Tokio::String(winData.classname);
				window.unique_title = Widgets::GetUniqueName(window.title, window.hwnd);
				window.hidden_title = std::string("##hidden_") + window.unique_title;

				// get window icon
				auto windowIcon = ViewAttachProcUtils::GetWindowIconTexture(window.hwnd);
				if (windowIcon != nullptr)
					window.icon = windowIcon;
				else
					window.icon = procData.icon;
			}

			if (procData.windows.size() > 1)
			{
				// sort the windows to display the longest 
				// title on top, prefer ones with an icon
				static const auto sortWindow = [](const WindowData& a, const WindowData& b) -> bool
				{
					if ((a.icon != nullptr && b.icon != nullptr) ||
						(a.icon == nullptr && b.icon == nullptr))
						return a.title.size() > b.title.size();

					return a.icon != nullptr;
				};

				std::sort(procData.windows.begin(), procData.windows.end(), sortWindow);
			}
		}
	}
	catch (Tokio::Exception& e)
	{
		e.Log("Cannot get window list");
	}

	// manually trigger the sort because we
	// have reset the process list
	m_table.Sort();

	m_timeLastRefresh = ImGui::GetTime();

	// restore the selected process
	if (oldSelectedPID != 0) SelectProcessByPid(oldSelectedPID);
}