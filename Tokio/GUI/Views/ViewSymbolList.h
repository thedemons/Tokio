﻿#pragma once
#ifndef TOKIO_GUI_VIEW_SYMBOLIST_H
#define TOKIO_GUI_VIEW_SYMBOLIST_H

#include "ViewSymbolList.h"
#include "../MainView.h"
#include "../Widgets/Widgets.hpp"
#include "../Widgets/WTextInput.h"
#include "../Widgets/WTreeTable.hpp"

class ViewSymbolList : public BaseView
{
private:
	class ModuleNode : public Widgets::TreeTable<ModuleNode>::NestedNode
	{
		using NestedNode::NestedNode;
	public:

		bool isModule = false;
		POINTER address = 0;

		// for modules
		std::string moduleNameA;
		std::string modulePathA;
		std::wstring moduleNameW;
		std::wstring modulePathW;
		PE_Data pe;

		// for symbols
		UINT ordinal = 0;
		std::string functionName;

		bool isHidden = false;		// true to hide this node, if the filter is on
		bool m_lastOpen = false;	// store the open state before start filtering, back it up if we don't filter anymore, only for modules

	};


	Widgets::TreeTable<ModuleNode> m_table;			// main table
	Widgets::TextInput m_textFilter;				// text input for filtering modules
	std::vector<ModuleNode> m_moduleList;

	static Widgets::TreeTable<ModuleNode>::Execution
		TableRenderCallback(
			Widgets::TreeTable<ModuleNode>* table,
			ModuleNode& node,
			size_t index,
			size_t level,
			void* UserData
		);

	static void TableInputCallback(Widgets::TreeTable<ModuleNode>* table, ModuleNode* node, size_t index, void* UserData);

	static void TableSortCallback(Widgets::TreeTable<ModuleNode>* table, size_t column, ImGuiSortDirection direction, void* UserData);

	static void TablePopupRenderCallback(Widgets::TreeTable<ModuleNode>* table, ModuleNode* node, size_t index, void* UserData);
	static void FilterEditCallback(Widgets::TextInput* tinput, ImGuiInputTextCallbackData* data, void* UserData);

public:
	ViewSymbolList();

	void Render(bool& bOpen) override;

	// update the module data
	void Update(const std::shared_ptr<ProcessData>& targetProcess) override;
};

#endif