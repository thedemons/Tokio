#pragma once
#ifndef TOKIO_GUI_VIEWS_DISASSEMBLER_H
#define TOKIO_GUI_VIEWS_DISASSEMBLER_H
#include "GUI/MainView.h"
#include "GUI/Widgets/WTable.h"
#include "GUI/Widgets/WTextInput.h"

class ViewDisassembler : public BaseView
{
private:
	// Table Callback
	static Widgets::Table::Execution TableRenderCallback(Widgets::Table* table, size_t index, void* UserData);
	static void TableInputCallback(Widgets::Table* table, size_t index, void* UserData);
	static void TablePopupRenderCallback(Widgets::Table* table, size_t index, void* UserData);

	static void PopupNavigateRenderCallback(Widgets::Popup* table, void* OpenUserData, void* UserData);
	//static void PopupEditInsRenderCallback(Widgets::Table* table, void* OpenUserData, void* UserData);

private:

	struct ViewDisasmData
	{
		BYTE bytes[24]{0};                             // raw bytes of the instruction
		size_t length      = 0ull;                  // length of the instruction, in bytes
		POINTER address    = 0ull;                  // virtual address of the target processs
		POINTER refAddress = 0ull;			        // the jump/call/mov... address, can be nullptr
		std::string addressSymbol;                  // formatted address
		std::string mnemonic;						// formatted mnemonic of the instruction (main operand)
		std::string instruction;                    // formatted instruction
		bool isBaseOffset = false;					// is this a base offset? (start of a module or function)
		bool isRefPointer = false;					// is the reference address a pointer to something, e.g. mov eax, [refAddress]
		std::string comment;
	};

	Widgets::Table m_table;							// main table
	Widgets::Popup m_popupNavigate;					// popup for "Go to address"
	Widgets::TextInput m_popupNavigateTextInput;		// text input for popup "Go to address"
	Widgets::Popup m_popupEditIns;					// popup for "Edit instruction" | NOT IMPLEMENTED YET

	double m_timeLastRefresh = 0.f;					// last refresh time, for refreshing the process list every x ms
	double m_refreshInterval = 1.f;					// refresh every 1000ms

	// virtual address in the target process
	POINTER m_pVirtualBase;

	// disassembled instructions
	std::vector<ViewDisasmData> m_disasmData;


	struct PrivateShortcutData
	{
		bool bCopyAddress = false;
		bool bFollowAddress = false;
		bool bGotoAddress = false;
		bool bOpenInMemory = false;
		bool bAddToWatchList = false;

	} m_shortcutData;

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