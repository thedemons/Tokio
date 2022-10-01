#pragma once
#ifndef TOKIO_GUI_VIEW_ATTACH_PROC_H
#define TOKIO_GUI_VIEW_ATTACH_PROC_H

#include "GUI/MainView.h"
#include "GUI/Widgets/WTable.h"
#include "GUI/Widgets/WTextInput.h"

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
		HWND hwnd = 0;								// window handle
		DWORD pid = 0;								// associated process
		std::string title;							// window title
		std::string classname;						// window class name
		std::string unique_title; 					// for tree node rendering
		std::string hidden_title; 					// for tree node rendering
		ID3D11ShaderResourceView* icon = nullptr;	// window icon texture
	};

	struct ProcessListData
	{
		DWORD pid = 0;								// process pid
		std::string  name;							// process name
		std::string  path;							// file path
		std::wstring wname;							// wide process name
		std::wstring wpath;							// wide file path
		uint64_t creationTime = 0;					// creation time
		PROCESSENTRY32W entry{};					// just in case
		
		std::vector<WindowData> windows;			// associated windows
		ID3D11ShaderResourceView* icon = nullptr;	// process icon texture

		bool isHidden = false;						// is this item hidden? (the filter is on)
	};

	Widgets::Table m_table;							// main table
	Widgets::TextInput m_textFilter;				// text input for filtering processes

	bool m_isOpenned = false;						// is the popup currenly openned
	std::vector<ProcessListData> m_processList;			// list of all process

	double m_timeLastRefresh = 0.f;					// last refresh time, for refreshing the process list every x ms
	double m_refreshInterval = 1111.f;					// refresh every 1000ms

	std::unordered_map<DWORD, ProcessListData> m_processCache;

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
	void SelectProcessByPid(DWORD pid);
	void GetProcessList();
};

#endif