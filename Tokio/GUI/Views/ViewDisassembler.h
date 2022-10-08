#pragma once
#ifndef TOKIO_GUI_VIEWS_DISASSEMBLER_H
#define TOKIO_GUI_VIEWS_DISASSEMBLER_H
#include "GUI/MainView.h"
#include "GUI/Widgets/WTable.h"
#include "GUI/Widgets/WTextInput.h"
#include "GUI/ImGui/custom/TokenizedText.h"

#include "Engine/EngineAnalyzerDef.h"
#include "Engine/Analyzer/BaseAnalyzer.h"
#include "GUI/Views/DisassemblerGraph.h"


class ViewDisassembler : public BaseView
{
private:
	// Table Callback
	static Widgets::Table::Execution TableRenderCallback(Widgets::Table* table, size_t index, void* UserData);
	static void TableInputCallback(Widgets::Table* table, size_t index, void* UserData);
	static void TablePopupRenderCallback(Widgets::Table* table, size_t index, void* UserData);

	static void PopupNavigateRenderCallback(Widgets::Popup* table, void* OpenUserData, void* UserData);
	//static void PopupEditInsRenderCallback(Widgets::Table* table, void* OpenUserData, void* UserData);

	int RenderScrollBar();
private:

	Widgets::Table m_table;							// main table
	Widgets::Popup m_popupNavigate;					// popup for "Go to address"
	Widgets::TextInput m_popupNavigateTextInput;	// text input for popup "Go to address"
	Widgets::Popup m_popupEditIns;					// popup for "Edit instruction" | NOT IMPLEMENTED YET

	double m_timeLastRefresh = 0.f;					// last refresh time, for refreshing the process list every x ms
	double m_refreshInterval = 1.f;				    // refresh every 1000ms


	// virtual address in the target process
	POINTER m_pVirtualBase;


	AnalyzedData m_analyzedData;


	// the start offset in the instruction list
	// for example: if we go to the address 0x400, it will read
	// the buffer at 0x390 instead of 0x400, to avoid parsing mid-instruction bytes
	// this offset indicate the starting position of address 0x400
	size_t m_instructionOffset = 0;

	// the buffer to store read memory
	std::vector<byte_t> m_memoryBuffer;


	bool m_scrollBarDragging = false;
	double m_scrollDragInterval = 0.0;

	bool m_isGraphMode = false;

	DisassemblerGraph m_graph;


	std::vector<POINTER> m_addressHistory;
	size_t m_addressHistoryIndex = UPTR_UNDEFINED;

	// if the selected row in the table were 0 and we scroll down
	// it means subtract the index so it will be negative, but that
	// is invalid, so we clear the selected index and remember where
	// we selected, restore it back if we are scrolling back up
	size_t m_rememberedSelectedIndex = 0;

private:
	void HandleShortcuts();
	void HandleScrolling();
	void RenderReferenceArrow();
	void Disassemble(Engine::AnalyzerFlags flags = Engine::AnalyzerFlags_::None);

public:
	ViewDisassembler();
	_CONSTEXPR20 bool isAllowMultipleViews()  const override
	{
		return true;
	}

	void Render(bool& bOpen) override;
	void OnAttach(const std::shared_ptr<ProcessData>& targetProcess) override;
	void OnDetach() override;


	void GoToAddress(POINTER address);

	// get the instruction at index with the m_instructionOffset added
	// use this safely, not to overflow the index
	_NODISCARD _CONSTEXPR20 AnalyzedInstruction& GetInstructionAt(size_t index)
	{
		return m_analyzedData.instructions[m_instructionOffset + index];
	}
};

#endif // !TOKIO_GUI_VIEWS_DISASSEMBLER_H