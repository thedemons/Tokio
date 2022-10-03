#pragma once
#include "stdafx.h"
#include "ViewDisassembler.h"

#include "imgui_custom.hpp"
#include "GUI/Widgets/Widgets.hpp"
#include "Resources/FontAwesomeImpl.h"
#include "GUI/Themes.hpp"

#include "Engine/Engine.h"

// Table Callback
Widgets::Table::Execution
ViewDisassembler::TableRenderCallback(Widgets::Table* table, size_t index, void* UserData)
{
	ViewDisassembler* pThis = static_cast<ViewDisassembler*>(UserData);
	auto& insData = pThis->m_instructionList[index];
	ImVec2 cursorOffset(0.f, 0.f);

	if (insData.isBaseOffset)
	{
		cursorOffset.y += 25.f;
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 10.f));
		ImGui::PushFont(MainApplication::FontMonoBigBold);
		ImGui::Text("%s", insData.addressSymbol.c_str());
		ImGui::PopFont();
	}

	// draw symbolic address
	if (insData.addressSymbol.size() > 0 )
	{
		ImGui::TextUnformatted(insData.addressSymbol.c_str(), insData.addressSymbol.c_str() + insData.addressSymbol.size());
	}
	else
	{
		
		ImGui::PushStyleColor(ImGuiCol_Text, Settings::data.theme.disasm.Address);
		ImGui::Text("%llX", insData.address);
		ImGui::PopStyleColor();
	}


	// bytes
	table->NextColumn();
	ImGui::SetCursorPos(ImGui::GetCursorPos() + cursorOffset);


	// not readable address, draw a "??"
	if (insData.isNotReadable)
	{
		table->SetColumnIndex(2);
		static std::string notReadableColored = "??"_c(Settings::data.theme.disasm.Invalid);
		ImGui::TextUnformatted(notReadableColored.c_str(), notReadableColored.c_str() + notReadableColored.size());
	}
	else
	{

		for (size_t i = 0; i < insData.length; i++)
		{
			ImGui::Text("%02X", insData.bytes[i]);
			//ImGui::TextColored(ImVec4(ThemeSettings::data.disasm.Bytes), "%02X", insData.bytes[i]);
			ImGui::SameLine();
		}

		// opcode
		table->NextColumn();
		ImVec2 cursorPos = ImGui::GetCursorPos() + cursorOffset;
		ImGui::SetCursorPos(cursorPos);

		ImGui::Text(insData.mnemonic.c_str(), insData.mnemonic.c_str() + insData.mnemonic.size());

		ImGui::SetCursorPos(cursorPos + ImVec2(55.f, 0.f));
		ImGui::Text(insData.instruction.c_str(), insData.instruction.c_str() + insData.instruction.size());

		// comment
		if (insData.comment.size() > 0)
		{
			table->NextColumn();
			ImGui::Text("%s", insData.comment.c_str());
		}
	}

	return Widgets::Table::Execution::Continue;
};

void ViewDisassembler::TableInputCallback(Widgets::Table* table, size_t index, void* UserData)
{
	ViewDisassembler* pThis = static_cast<ViewDisassembler*>(UserData);

	// if any row is hovered
	if (index != UPTR_UNDEFINED)
	{

	}

	if (ImGui::IsKeyPressed(ImGuiKey_Space))
	{
		if (auto& selected = table->GetSelectedItems(); selected.size() > 0)
		{
			POINTER refAddress = pThis->m_instructionList[selected[0]].refAddress;
			if (refAddress)
			{

				pThis->GoToAddress(refAddress);
			}
		}
	}
}

void ViewDisassembler::TablePopupRenderCallback(Widgets::Table* table, size_t index, void* UserData)
{
	ViewDisassembler* pThis = static_cast<ViewDisassembler*>(UserData);

	// popup events are guaranteed to have at least one selected item
	index = table->GetSelectedItems()[0];


	pThis->m_shortcutData.bCopyAddress      = ImGui::MenuItem(ICON_FA_CLIPBOARD u8" Copy address",        "Ctrl+C");
	ImGui::Separator();

	pThis->m_shortcutData.bFollowAddress    = ImGui::MenuItem(                  u8"⏩ Follow",				"Space");
	pThis->m_shortcutData.bGotoAddress      = ImGui::MenuItem(                  u8"⏩ Go to address",      "Ctrl+G");
	ImGui::Separator();

	pThis->m_shortcutData.bOpenInMemory     = ImGui::MenuItem(ICON_MEMORY_VIEW  u8" Open in Memory View", "Ctrl+B");
	pThis->m_shortcutData.bAddToWatchList   = ImGui::MenuItem(ICON_WATCH_LIST   u8" Add to Watch List",   "Ctrl+E");

}

void ViewDisassembler::PopupNavigateRenderCallback(Widgets::Popup* popup, void* OpenUserData, void* UserData)
{
	ViewDisassembler* pThis = static_cast<ViewDisassembler*>(UserData);

	ImGui::SetNextItemWidth(-1);
	bool isEnter = pThis->m_popupNavigateTextInput.Render();

	if (isEnter)
	{
		POINTER address = Engine::Symbol()->SymbolToAddress(pThis->m_popupNavigateTextInput.str_strip()).value();
		popup->Close();
		pThis->GoToAddress(address);
		pThis->m_popupNavigateTextInput.Clear();
	}

}

ViewDisassembler::ViewDisassembler()
{
	m_title = ICON_DISASSEMBLER u8" Disassembler";

	Widgets::Table::Desc desc;
	desc.Name = "##TableDisassembler";

	desc.InputUserData = this;
	desc.RenderUserData = this;
	desc.PopupRenderUserData = this;

	desc.InputCallback = TableInputCallback;
	desc.RenderCallback = TableRenderCallback;
	desc.PopupRenderCallback = TablePopupRenderCallback;

	desc.RowFont = MainApplication::FontMonoRegular;

	desc.Flags = ImGuiTableFlags_BordersOuter | // outer borders
				 ImGuiTableFlags_SortTristate | // disable sort
				 ImGuiTableFlags_Hideable     | // hide the columns
				 ImGuiTableFlags_Resizable    | // resizable coulmns by default
				 ImGuiTableFlags_Reorderable  ; // re-order the coulmns

	m_table.Setup(desc);

	m_table.AddColumn("Address", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, 120.f);
	m_table.AddColumn("Bytes");
	m_table.AddColumn("Opcode");
	m_table.AddColumn("Comments");


	Widgets::Popup::Desc popupNavigateDesc;
	popupNavigateDesc.name = "Go to address";
	popupNavigateDesc.RenderCallback = PopupNavigateRenderCallback;
	popupNavigateDesc.RenderUserData = this;
	popupNavigateDesc.initialSize = { 300.f, 100.f };
	popupNavigateDesc.Flags = ImGuiWindowFlags_NoSavedSettings ;
	popupNavigateDesc.WidgetFlags = Widgets::PopupFlags::PopupModal | Widgets::PopupFlags::CloseOnEscape;

	m_popupNavigate.Setup(popupNavigateDesc);

	Widgets::TextInput::Desc popupNavigateTiDesc;
	popupNavigateTiDesc.Flags = ImGuiInputTextFlags_EnterReturnsTrue;
	popupNavigateTiDesc.Label = "##popupNavigateTextInput";
	popupNavigateTiDesc.Hint = "Enter an address, including symbols, modules";
	m_popupNavigateTextInput.Setup(popupNavigateTiDesc);

}

void ViewDisassembler::Render(bool& bOpen)
{
	if (!bOpen) return;

	double currentTime = ImGui::GetTime();
	if (currentTime - m_timeLastRefresh > m_refreshInterval)
	{
		m_timeLastRefresh = currentTime;
		//GoToAddress(m_pVirtualBase);
	}

	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 0,0 });
	ImGui::Begin(Title().c_str(), &bOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PopStyleVar();

	m_table.Render(m_instructionList.size());

	ImGui::End();

	m_popupNavigate.Render();

	// handle shortcuts
	if (ImGui::IsKeyPressed(ImGuiKey_G) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && !m_popupNavigate.IsOpen())
	{
		m_popupNavigate.Open();
		m_popupNavigateTextInput.Focus();
	}

	if (m_shortcutData.bCopyAddress)
	{
		auto& selectedItems = m_table.GetSelectedItems();
		if (selectedItems.size() > 0)
		{
			if (auto result = common::BhClipboardCopy(m_instructionList[selectedItems[0]].address); result.has_error())
				result.error().show();
		}

		m_shortcutData.bCopyAddress = false;
	}
}


void ViewDisassembler::OnAttach(const std::shared_ptr<ProcessData>& targetProcess)
{
	m_SymbolHandler = Engine::Symbol();
	GoToAddress(targetProcess->baseModule->base);
}

void ViewDisassembler::OnDetach()
{
	m_instructionList.clear();
	m_SymbolHandler.reset();
}

std::string FormatSymbolAddress(
	POINTER address,
	const ResultGetSymbol& resultGetModuleSymbol,
	bool* isBaseOffset = nullptr)
{
	static char symbol[1024];
	static std::string regularAddress = "%llX"_c(Settings::data.theme.disasm.Address);

	static std::string formatProcModBase = 
		"%s"_c(Settings::data.theme.disasm.Module);

	static std::string formatProcMod = 
		"%s"_c(Settings::data.theme.disasm.Module) +
		"+"_c(Settings::data.theme.disasm.Delimeter) +
		"%llX"_c(Settings::data.theme.disasm.Displacement);

	static std::string formatFunc = 
		"%s"_c(Settings::data.theme.disasm.Module) 
		+ "."_c(Settings::data.theme.disasm.Delimeter)
		+ "%s"_c(Settings::data.theme.disasm.Function)
		+ "+"_c(Settings::data.theme.disasm.Delimeter) 
		+ "%llX"_c(Settings::data.theme.disasm.Displacement);

	static std::string formatFuncBase = 
		"%s"_c(Settings::data.theme.disasm.Module) +
		"."_c(Settings::data.theme.disasm.Delimeter) +
		"%s"_c(Settings::data.theme.disasm.Function);

	// parse the address normally
	if (!resultGetModuleSymbol.has_value())
	{
		sprintf_s(symbol, regularAddress.c_str(), address);
		return symbol;
	}

	if (!resultGetModuleSymbol.has_symbol())
	{
		auto pModule = resultGetModuleSymbol.Module();
		if (address != pModule->base)
		{
			sprintf_s(symbol, formatProcMod.c_str(), pModule->moduleNameA.c_str(), address - pModule->base);
			if (isBaseOffset) *isBaseOffset = false;
		}
		else
		{
			sprintf_s(symbol, formatProcModBase.c_str(), pModule->moduleNameA.c_str(), address - pModule->base);
			if (isBaseOffset) *isBaseOffset = true;
		}
	}
	else
	{
		auto pModule = resultGetModuleSymbol.Module();
		auto pSymbol = resultGetModuleSymbol.Symbol();

		std::string modShortName = pModule->moduleNameA;
		auto find = modShortName.rfind('.');
		if (find != std::string::npos)
			modShortName = modShortName.substr(0, find);

		POINTER offsetFromVA = address - (pModule->base + pSymbol->offset);

		if (offsetFromVA > 0)
		{
			sprintf_s(symbol, formatFunc.c_str(), modShortName.c_str(), pSymbol->name.c_str(), offsetFromVA);
			if (isBaseOffset) *isBaseOffset = false;
		}
		else
		{
			sprintf_s(symbol, formatFuncBase.c_str(), modShortName.c_str(), pSymbol->name.c_str());
			if (isBaseOffset) *isBaseOffset = true;
		}
	}

	return symbol;
}


void ViewDisassembler::DisassembleRegion(size_t offset, size_t size)
{
	auto disasmResult = Engine::Disassembler()->Disasm(m_pVirtualBase + offset, m_memoryBuffer.data() + offset, size);
	assert(disasmResult.has_value() && "Disassemble failed");

	auto& instructions = disasmResult.value();

	size_t currentInstructionIndex = m_instructionList.size();
	size_t instructionIndex = 0;
	m_instructionList.resize(m_instructionList.size() + instructions.size());

	auto walkContext = m_SymbolHandler->AddressSymbolWalkInit();
	
	for (auto& disasmData : disasmResult.value())
	{
		auto resultGetModuleSymbol = m_SymbolHandler->AddressSymbolWalkNext(walkContext, disasmData.address);

		auto& viewDisasmData = m_instructionList[currentInstructionIndex + instructionIndex++];
		viewDisasmData.address = disasmData.address;
		viewDisasmData.addressSymbol = FormatSymbolAddress(disasmData.address, resultGetModuleSymbol, &viewDisasmData.isBaseOffset);
		viewDisasmData.refAddress = disasmData.refAddress;
		viewDisasmData.isRefPointer = disasmData.isRefPointer;
		viewDisasmData.length = disasmData.length;

		// here we parse the tokenized operands into colored text
		for (auto& operand : disasmData.operands)
		{
			// skip the menomonic and invalid operands
			if (IsOperandMnemonic(operand.type) || operand.type == DisasmOperandType::Invalid) continue;

			// don't colorize white spaces
			if (operand.type == DisasmOperandType::WhiteSpace)
			{
				viewDisasmData.instruction += " ";
			}
			// it has a reference address, draw it as a symbol
			else if (viewDisasmData.refAddress != 0ull && operand.type == DisasmOperandType::AddressAbs)
			{
				auto resultGetModuleSymbol = m_SymbolHandler->AddressToModuleSymbol(viewDisasmData.refAddress);
				viewDisasmData.instruction += FormatSymbolAddress(viewDisasmData.refAddress, resultGetModuleSymbol);
			}
			// it's just a regular operand
			else
			{
				DWORD color = ThemeSettings::GetDisasmColor(operand.type);
				viewDisasmData.instruction += ImGuiCustomString(operand.value)(color);
			}
		}

		// if it's is something like `mov rax, [0x12345]`
		// we read the value at the address 0x12345 and add it as a comment
		if (viewDisasmData.isRefPointer)
		{
			auto resultRead = Engine::ReadMem<POINTER>(viewDisasmData.refAddress);
			if (resultRead.has_value())
			{
				POINTER refPointer = resultRead.value();
				auto resultGetModuleSymbol = m_SymbolHandler->AddressToModuleSymbol(refPointer);
				viewDisasmData.comment = FormatSymbolAddress(refPointer, resultGetModuleSymbol);
			}
		}

		DWORD color = ThemeSettings::GetDisasmColor(disasmData.mnemonic.type);
		viewDisasmData.mnemonic = ImGuiCustomString(disasmData.mnemonic.value)(color);
	}
}

void ViewDisassembler::Disassemble()
{
	m_instructionList.clear();
	m_instructionOffset = 0;

	if (!Engine::IsAttached()) return;

	POINTER startAddress = m_pVirtualBase - 0x10;
	size_t bufferSize = m_memoryBuffer.size();

	// to avoid disassembling mid-instruction
	std::vector<MemoryRegion> regions;
	auto resultRead = Engine::ReadMemSafe(startAddress, m_memoryBuffer.data(), bufferSize, regions);
	
	assert(resultRead.has_value() && "ViewDisassembler Cannot read memory");


	// no readable memory, set all instructions to invalid
	if (regions.size() == 0)
	{
		m_instructionList.resize(100);

		for (auto& instruction : m_instructionList)
		{
			instruction.address = startAddress++;
			instruction.isNotReadable = true;
		}
	}
	else
	{
		auto regionIter = regions.begin();

		// loop through the buffer, if it's not readable then push invalid instructions, else disassemble it
		size_t offset = startAddress;
		while (offset < startAddress + bufferSize)
		{
			// push invalid instructions to the list if the memory is not readable
			if (offset < regionIter->start)
			{
				for (; offset < regionIter->start; offset++)
				{
					auto& instruction = m_instructionList.emplace_back();
					instruction.address = offset;
					instruction.isNotReadable = true;
				}
			}
			// disassemble valid region
			else
			{
				DisassembleRegion(regionIter->start - startAddress, regionIter->size);

				offset += regionIter->size;
				if (++regionIter == regions.end()) break;
			}
		}

		// there are more left after disassembling valid regions
		// push invalid instructions
		for (; offset < startAddress + bufferSize; offset++)
		{
			auto& instruction = m_instructionList.emplace_back();
			instruction.address = offset;
			instruction.isNotReadable = true;
		}
	}
}

// TODO: Fix this mess
void ViewDisassembler::GoToAddress(POINTER address)
{
	m_pVirtualBase = address;
	Disassemble();
}

//
//BYTE pBuffer[1024];
//auto readMemResult = Engine::ReadMem(address, pBuffer, sizeof(pBuffer));
//if (readMemResult.has_error())
//{
//	//readMemResult.error().show(L"Disassembler failed to read memory process");
//	//return;
//}
//
//auto disasmResult = Engine::Disassembler()->Disasm(address, pBuffer, sizeof(pBuffer));
//if (disasmResult.has_error())
//{
//	disasmResult.error().show(L"Error while disassembling the target process");
//
//}
//else
//{
//	m_instructionList.clear();
//	m_instructionList.resize(disasmResult.value().size());
//
//	size_t index = 0;
//
//	auto symbolEngine = Engine::Symbol();
//	auto walkContext = symbolEngine->AddressSymbolWalkInit();
//
//	for (auto& disasmData : disasmResult.value())
//	{
//		auto resultGetModuleSymbol = symbolEngine->AddressSymbolWalkNext(walkContext, disasmData.address);
//
//		auto& viewDisasmData = m_instructionList[index];
//		viewDisasmData.address = disasmData.address;
//		viewDisasmData.addressSymbol = FormatSymbolAddress(disasmData.address, resultGetModuleSymbol, &viewDisasmData.isBaseOffset);
//		viewDisasmData.refAddress = disasmData.refAddress;
//		viewDisasmData.isRefPointer = disasmData.isRefPointer;
//		viewDisasmData.length = disasmData.length;
//
//		memcpy_s(viewDisasmData.bytes, sizeof(viewDisasmData.bytes), disasmData.bytes, sizeof(disasmData.bytes));
//
//		// here we parse the tokenized operands into colored text
//		for (auto& operand : disasmData.operands)
//		{
//			// skip the menomonic and invalid operands
//			if (IsOperandMnemonic(operand.type) || operand.type == DisasmOperandType::Invalid) continue;
//
//			// don't colorize white spaces
//			if (operand.type == DisasmOperandType::WhiteSpace)
//			{
//				viewDisasmData.instruction += " ";
//			}
//			// it has a reference address, draw it as a symbol
//			else if (viewDisasmData.refAddress != 0ull && operand.type == DisasmOperandType::AddressAbs)
//			{
//				auto resultGetModuleSymbol = symbolEngine->AddressToModuleSymbol(viewDisasmData.refAddress);
//				viewDisasmData.instruction += FormatSymbolAddress(viewDisasmData.refAddress, resultGetModuleSymbol);
//			}
//			// it's just a regular operand
//			else
//			{
//				DWORD color = ThemeSettings::GetDisasmColor(operand.type);
//				viewDisasmData.instruction += ImGuiCustomString(operand.value)(color);
//			}
//		}
//
//		if (viewDisasmData.isRefPointer)
//		{
//			auto resultRead = Engine::ReadMem<POINTER>(viewDisasmData.refAddress);
//			if (resultRead.has_value())
//			{
//				POINTER refPointer = resultRead.value();
//				auto resultGetModuleSymbol = symbolEngine->AddressToModuleSymbol(refPointer);
//				viewDisasmData.comment = FormatSymbolAddress(refPointer, resultGetModuleSymbol);
//			}
//		}
//
//		DWORD color = ThemeSettings::GetDisasmColor(disasmData.mnemonic.type);
//		viewDisasmData.mnemonic = ImGuiCustomString(disasmData.mnemonic.value)(color);
//
//		index++;
//	}
//
//	m_table.AddSelectedItem(0);
//	m_table.SetScroll(0.f);
//
//	m_pVirtualBase = address;
//}