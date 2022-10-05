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

	struct ViewInstructionData
	{
		DisasmOperandType mnemonic_type = DisasmOperandType::Invalid;
		size_t bufferOffset = 0;				// the position in the m_memoryBuffer
		size_t length       = 0;				// length of the instruction, in bytes
		POINTER address     = 0;				// virtual address of the target processs
		POINTER refAddress  = 0;				// the jump/call/mov... address, can be nullptr
		POINTER refValue    = 0;				// if isRefPointer is true, this contains the value of [refAddress]
		ImGui::TokenizedText addressSymbol;		// formatted address
		ImGui::TokenizedText mnemonic;			// formatted mnemonic of the instruction (main operand)
		ImGui::TokenizedText instruction;		// formatted instruction
		bool isBaseOffset   = false;			// is this a base offset? (start of a module or function)
		bool isRefPointer   = false;			// is the reference address a pointer to something, e.g. mov eax, [refAddress]
		bool isNotReadable  = false;			// if the memory is not readable (display as "??")
		ImGui::TokenizedText comment;

		// for rendering references
		ImVec2 cursorPos{ 0.f, 0.f };
		float refDepthLevel = 0.f;

		// check if the instruction was rendered this frame
		// we skipped rendering it for optimization
		//bool isRendered = false;

		// index of the instruction this instruction refer to
		size_t referenceIndex = UPTR_UNDEFINED;

		// list of the instructions the refer to this instruction
		std::vector<size_t> refererIndexes;

		// address of the instruction the refer to this instruction
		//POINTER refererAddress = 0;

		bool operator==(const ViewInstructionData& v) {
			return address == v.address;
		};
		bool operator==(const POINTER& v) {
			return address == v;
		};
	};


	Widgets::Table m_table;							// main table
	Widgets::Popup m_popupNavigate;					// popup for "Go to address"
	Widgets::TextInput m_popupNavigateTextInput;	// text input for popup "Go to address"
	Widgets::Popup m_popupEditIns;					// popup for "Edit instruction" | NOT IMPLEMENTED YET

	double m_timeLastRefresh = 0.f;					// last refresh time, for refreshing the process list every x ms
	double m_refreshInterval = 0.25f;				// refresh every 250ms


	// virtual address in the target process
	POINTER m_pVirtualBase;

	// disassembled instructions
	//std::vector<ViewInstructionData> m_instructionList;

	// list of instruction that has a reference pointer
	//std::vector<std::reference_wrapper<ViewInstructionData>> m_referenceList;

	AnalyzedData m_analyzedData;


	// the start offset in the instruction list
	// for example: if we go to the address 0x400, it will read
	// the buffer at 0x390 instead of 0x400, to avoid parsing mid-instruction bytes
	// this offset indicate the starting position of address 0x400
	size_t m_instructionOffset = 0;

	// the buffer to store read memory
	std::vector<BYTE> m_memoryBuffer;


	bool m_scrollBarDragging = false;
	double m_scrollDragInterval = 0.0;

	bool m_isGraphMode = false;

	DisassemblerGraph m_graph;


	std::vector<POINTER> m_addressHistory;
	size_t m_addressHistoryIndex = UPTR_UNDEFINED;

private:
	void HandleShortcuts();
	void HandleScrolling();
	void RenderReferenceArrow();
	void Disassemble();

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