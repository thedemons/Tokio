#pragma once
#ifndef TOKIO_GUI_VIEWS_DISASSEMBLER_H
#define TOKIO_GUI_VIEWS_DISASSEMBLER_H
#include "GUI/MainView.h"
#include "GUI/Widgets/WTable.h"
#include "GUI/Widgets/WTextInput.h"
//#include "Engine/Disassembler/BaseDisassembler.hpp"

// use BaseSymbol without including Engine.h
namespace Engine { class BaseSymbol; }

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

	struct ViewInstructionData
	{
		BYTE bytes[24]{0};                          // raw bytes of the instruction
		size_t length      = 0ull;                  // length of the instruction, in bytes
		POINTER address    = 0ull;                  // virtual address of the target processs
		POINTER refAddress = 0ull;			        // the jump/call/mov... address, can be nullptr
		std::string addressSymbol;                  // formatted address
		std::string mnemonic;						// formatted mnemonic of the instruction (main operand)
		std::string instruction;                    // formatted instruction
		bool isBaseOffset = false;					// is this a base offset? (start of a module or function)
		bool isRefPointer = false;					// is the reference address a pointer to something, e.g. mov eax, [refAddress]
		bool isNotReadable = false;					// if the memory is not readable (display as "??")
		std::string comment;
	};

	Widgets::Table m_table;							// main table
	Widgets::Popup m_popupNavigate;					// popup for "Go to address"
	Widgets::TextInput m_popupNavigateTextInput;	// text input for popup "Go to address"
	Widgets::Popup m_popupEditIns;					// popup for "Edit instruction" | NOT IMPLEMENTED YET

	double m_timeLastRefresh = 0.f;					// last refresh time, for refreshing the process list every x ms
	double m_refreshInterval = 1.f;					// refresh every 1000ms


	// need a symbol engine to symbolize the opcodes
	// get the symbol on OnAttach() and destroy it on OnDetach()
	std::shared_ptr<Engine::BaseSymbol> m_SymbolHandler = nullptr;

	// virtual address in the target process
	POINTER m_pVirtualBase;

	// disassembled instructions
	std::vector<ViewInstructionData> m_instructionList;

	// the start offset in the instruction list
	// for example: if we go to the address 0x400, it will read
	// the buffer at 0x390 instead of 0x400, to avoid parsing mid-instruction bytes
	// this offset indicate the starting position of address 0x400
	size_t m_instructionOffset = 0;

	// the buffer to store read memory
	std::array<BYTE, 512> m_memoryBuffer;

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

	void OnAttach(const std::shared_ptr<ProcessData>& targetProcess) override;
	void OnDetach() override;

	void Disassemble();

	// offset: offset from the start of the buffer
	// size: size of the region
	void DisassembleRegion(size_t offset, size_t size);
	void GoToAddress(POINTER address);


};

#endif // !TOKIO_GUI_VIEWS_DISASSEMBLER_H