#pragma once
#ifndef TOKIO_GUI_VIEWS_DISASSEMBLER_H
#define TOKIO_GUI_VIEWS_DISASSEMBLER_H
#include "GUI/MainView.h"
#include "GUI/Widgets/WTable.h"

class ViewDisassembler : public BaseView
{
private:
	// Table Callback
	static Widgets::Table::Execution
		TableRenderCallback(Widgets::Table* table, size_t index, void* UserData);

	static void TableInputCallback(Widgets::Table* table, size_t index, void* UserData);

	static void TablePopupRenderCallback(Widgets::Table* table, size_t index, void* UserData);
private: // members
	Widgets::Table m_table;							// main table

	double m_timeLastRefresh = 0.f;					// last refresh time, for refreshing the process list every x ms
	double m_refreshInterval = 1111.f;					// refresh every 1000ms

	// virtual address in the target process
	POINTER m_pVirtualBase;

	// disassembled instructions
	std::vector<DisasmData> m_Instructions;

public:
	ViewDisassembler();
	_CONSTEXPR20 bool isAllowMultipleViews()  const override
	{
		return true;
	}

	void Render(bool& bOpen) override;

public:

	void Update(const std::shared_ptr<ProcessData>& targetProcess) override;
	void GoToAddress(POINTER address);


};

#endif // !TOKIO_GUI_VIEWS_DISASSEMBLER_H