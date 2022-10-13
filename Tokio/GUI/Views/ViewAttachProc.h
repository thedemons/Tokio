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
#ifndef TOKIO_GUI_VIEWS_ATTACH_PROC_H
#define TOKIO_GUI_VIEWS_ATTACH_PROC_H

#include "GUI/MainView.h"
#include "GUI/Widgets/WTable.h"
#include "GUI/Widgets/WTextInput.h"
#include "Common/TypeDef.h"

#include <map>

class ViewAttachProc : public BaseView
{
private: 
	// Table Callback
	static Widgets::Table::Execution
	TableRenderCallback(Widgets::Table* table, size_t index, void* UserData);

	static void TableSortCallback(Widgets::Table* table, size_t column, ImGuiSortDirection direction, void* UserData);

	static void TableInputCallback(Widgets::Table* table, size_t index, void* UserData);

	static void TablePopupRenderCallback(Widgets::Table* table, size_t index, void* UserData);

	static void FilterEditCallback(Widgets::TextInput* tinput, ImGuiInputTextCallbackData* data, void* UserData);

private: // members
	struct WindowData
	{
		PID pid = 0;								    // associated process
		WindowHandle hwnd = 0;					        // window handle
		std::string title;							    // window title
		std::string classname;						    // window class name
		std::string unique_title; 					    // for tree node rendering
		std::string hidden_title; 					    // for tree node rendering
		ImTextureID icon = nullptr;	                    // window icon texture
	};

	struct ProcessListData
	{
		PID pid = 0;								    // process pid
		std::string  name;							    // process name
		std::string  path;							    // file path
		std::wstring wname;							    // wide process name
		std::wstring wpath;							    // wide file path
		uint64_t creationTime = 0;					    // creation time
		ProcessEntry entry;			                    // just in case
		
		std::vector<WindowData> windows;			    // associated windows
		ImTextureID icon = nullptr;	                    // process icon texture

		bool isHidden = false;						    // is this item hidden? (the filter is on)
	};

	Widgets::Table m_table;							    // main table
	Widgets::TextInput m_textFilter;				    // text input for filtering processes

	bool m_isOpenned = false;						    // is the popup currenly openned
	std::vector<ProcessListData> m_processList;			// list of all process

	double m_timeLastRefresh = 0.f;					    // last refresh time, for refreshing the process list every x ms
	double m_refreshInterval = 0.5f;					// refresh every 500ms

	std::future<void> m_threadGetProc;
	std::mutex m_mutex;

	void ThreadGetProc();

public:
	ViewAttachProc();


	_CONSTEXPR20 bool isClosable() const  override
	{
		return false;
	}

	_CONSTEXPR20 bool isDefaultOpen()  const override
	{
		return false;
	}

	_CONSTEXPR20 bool isAllowMultipleViews()  const override
	{
		return false;
	}

	void Render(bool& bOpen) override;

private:

	bool IsItemFiltered(const ProcessListData& data, const std::string& filter);
	void SelectProcessByPid(PID pid);
	void GetProcessList();
};

#endif // !TOKIO_GUI_VIEWS_ATTACH_PROC_H