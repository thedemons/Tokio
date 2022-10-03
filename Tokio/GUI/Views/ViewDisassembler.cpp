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
	auto& insData = pThis->GetInstructionAt(index);
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

		// mnemonic
		table->NextColumn();

		// we save the cursor pos for rendering references arrow
		insData.cursorPos = ImGui::GetCursorPos() + cursorOffset;
		ImGui::SetCursorPos(insData.cursorPos);

		ImGui::Text(insData.mnemonic.c_str(), insData.mnemonic.c_str() + insData.mnemonic.size());

		// instructions
		ImGui::SetCursorPos(insData.cursorPos + ImVec2(55.f, 0.f));
		ImGui::Text(insData.instruction.c_str(), insData.instruction.c_str() + insData.instruction.size());

		// comment
		if (insData.comment.size() > 0)
		{
			table->NextColumn();
			ImGui::TextUnformatted(insData.comment.c_str(), insData.comment.c_str() + insData.comment.size());
		}
	}

	// calculate jump instruction
	if (insData.mnemonic_type == DisasmOperandType::mneJump ||
		insData.mnemonic_type == DisasmOperandType::mneJumpCondition)
	{
		// if the reference address inside the current instruction list
		if (insData.refAddress != 0ull &&
			insData.refAddress >= pThis->m_instructionList[pThis->m_instructionOffset].address &&
			insData.refAddress <= pThis->m_instructionList.back().address
			)
		{

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
			POINTER refAddress = pThis->GetInstructionAt(selected[0]).refAddress;
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

	m_table.Render(m_instructionList.size() - m_instructionOffset);
	ImVec2 windowPos = ImGui::GetWindowPos();

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

	auto* dl = ImGui::GetForegroundDrawList();

	for (auto it = m_instructionList.begin() + m_instructionOffset; it != m_instructionList.end(); it++)
	{
		if (it->referenceIndex != UPTR_UNDEFINED && it->referenceIndex >= m_instructionOffset)
		{
			auto& refInstruction = m_instructionList[it->referenceIndex];
			float length = fabs(it->cursorPos.y - refInstruction.cursorPos.y) * 0.05f;

			ImVec2 p1 = it->cursorPos + windowPos + ImVec2(0, 5);
			ImVec2 p2 = it->cursorPos + windowPos - ImVec2(length, -5);
			ImVec2 p3 = refInstruction.cursorPos + windowPos - ImVec2(length, -5);
			ImVec2 p4 = refInstruction.cursorPos + windowPos + ImVec2(0, 5);
			dl->AddBezierCubic(p1, p2, p3, p4, Settings::data.theme.disasm.mneJumpCondition, 0.5f);
		}
	}

	auto& io = ImGui::GetIO();
	if (io.MouseWheel != 0.f)
	{
		m_instructionOffset -= static_cast<size_t>(floor(io.MouseWheel));
		m_pVirtualBase = m_instructionList[m_instructionOffset].address;
		Disassemble();
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
	static std::string regularAddress = "%llX"_c(Settings::data.theme.disasm.AddressAbs);

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


// analyze the reference of the instruction (if it is a pointer e.g isRefPointer == true)
void ViewDisassembler::AnalyzeInstructionReference(ViewInstructionData& insData)
{
	static std::string formatString = "\"%s\""_c(Settings::data.theme.disasm.String);
	static std::string formatPointer = "[%llX]"_c(Settings::data.theme.disasm.AddressAbs);
	static std::string formatDecimal = "%d"_c(Settings::data.theme.disasm.Displacement);
	static char bufferFmt[1024];

	auto resultRead = Engine::ReadMem<POINTER>(insData.refAddress);
	if (resultRead.has_value())
	{
		POINTER refPointer = resultRead.value();
		insData.refValue = refPointer;

		// strip 4 bytes if the target is 32 bit
		if (Engine::Is32Bit()) refPointer &= 0xFFFFFFFF;

		auto resultGetModuleSymbol = m_SymbolHandler->AddressToModuleSymbol(refPointer);

		// format as a symbol
		if (resultGetModuleSymbol.has_value())
		{
			insData.comment = FormatSymbolAddress(refPointer, resultGetModuleSymbol);
			return;
		}

		// if it doesn't have a symbol, try to read it as a string
		static char stringBuffer[64]{};
		if (!Engine::ReadMem(insData.refAddress, stringBuffer, sizeof(stringBuffer) - 1).has_error())
		{
			// hardcoded 5 valid char to be defined as a string

			bool isValidString = strnlen_s(stringBuffer, 64) >= 5;
			bool isValidWString = false;
			if (!isValidString) isValidWString = wcsnlen_s(reinterpret_cast<wchar_t*>(stringBuffer), 31) >= 5;

			// it is a string!
			if (isValidString)
			{
				sprintf_s(bufferFmt, formatString.c_str(), stringBuffer);
				insData.comment = bufferFmt;
				return;
			}
			else if (isValidWString)
			{
				std::wstring wideString(reinterpret_cast<wchar_t*>(stringBuffer));
				sprintf_s(bufferFmt, formatString.c_str(), common::BhString(wideString).c_str());
				insData.comment = bufferFmt;
				return;
			}
		}

		// if it's not a string, then might it be a pointer?
		resultRead = Engine::ReadMem<POINTER>(refPointer);
		if (!resultRead.has_error())
		{
			sprintf_s(bufferFmt, formatPointer.c_str(), refPointer);
			insData.comment = bufferFmt;
			return;
		}

		// it it's not anything above, just format it as a decimal value

		sprintf_s(bufferFmt, formatDecimal.c_str(), refPointer);
		insData.comment = bufferFmt;
	}
}

void ViewDisassembler::AnalyzeCrossReference()
{
	static char bufferFmt[1024];
	std::string formatXRef = "XREF: %s "_c(Settings::data.theme.disasm.Xref);

	POINTER instructionStart = m_instructionList.front().address;
	POINTER instructionEnd = m_instructionList.back().address;

	// calculate the reference index, mainly used for jump and call pointer rendering
	for (auto it = m_instructionList.begin(); it != m_instructionList.end(); it++)
	{
		// if the reference address is in range of the current instructions
		if (it->refAddress != 0 &&
			instructionStart <= it->refAddress &&
			it->refAddress <= instructionEnd)
		{
			std::vector<ViewInstructionData>::iterator reference;

			// we are optimizing the search by doing this in a sorted list
			// will binary search be better?
			if (it->refAddress > it->address)
			{
				reference = std::find(it, m_instructionList.end(), it->refAddress);
			}
			else
			{
				reference = std::find(m_instructionList.begin(), it, it->refAddress);
			}

			if (reference != m_instructionList.end())
			{
				it->referenceIndex = reference - m_instructionList.begin();
				reference->refererIndex = it - m_instructionList.begin();
				sprintf_s(bufferFmt, formatXRef.c_str(), it->addressSymbol.c_str());
				reference->comment += bufferFmt;
			}
		}
	}
}

void ViewDisassembler::DisassembleRegion(POINTER pVirtualBase, const BYTE* pOpCodes, size_t size)
{
	auto disasmResult = Engine::Disassembler()->Disasm(pVirtualBase, pOpCodes, size);
	assert(disasmResult.has_value() && "Disassemble failed");

	auto& instructions = disasmResult.value();

	size_t currentInstructionIndex = m_instructionList.size();
	size_t instructionIndex = 0;
	m_instructionList.resize(m_instructionList.size() + instructions.size());

	auto walkContext = m_SymbolHandler->AddressSymbolWalkInit();
	
	for (auto& disasmData : disasmResult.value())
	{
		auto resultGetModuleSymbol = m_SymbolHandler->AddressSymbolWalkNext(walkContext, disasmData.address);

		auto& insData = m_instructionList[currentInstructionIndex + instructionIndex++];
		insData.address = disasmData.address;
		insData.addressSymbol = FormatSymbolAddress(disasmData.address, resultGetModuleSymbol, &insData.isBaseOffset);
		insData.refAddress = disasmData.refAddress;
		insData.isRefPointer = disasmData.isRefPointer;
		insData.length = disasmData.length;
		insData.mnemonic_type = disasmData.mnemonic.type;

		// here we parse the tokenized operands into colored text
		for (auto& operand : disasmData.operands)
		{
			// skip the menomonic and invalid operands
			if (IsOperandMnemonic(operand.type) || operand.type == DisasmOperandType::Invalid) continue;

			// don't colorize white spaces
			if (operand.type == DisasmOperandType::WhiteSpace)
			{
				insData.instruction += " ";
			}
			// it has a reference address, draw it as a symbol
			else if (insData.refAddress != 0ull && operand.type == DisasmOperandType::AddressAbs)
			{
				auto resultGetModuleSymbol = m_SymbolHandler->AddressToModuleSymbol(insData.refAddress);
				insData.instruction += FormatSymbolAddress(insData.refAddress, resultGetModuleSymbol);
			}
			// it's just a regular operand
			else
			{
				DWORD color = ThemeSettings::GetDisasmColor(operand.type);
				insData.instruction += ImGuiCustomString(operand.value)(color);
			}
		}

		// if it's is something like `mov rax, [0x12345]`
		if (insData.isRefPointer) AnalyzeInstructionReference(insData);

		DWORD color = ThemeSettings::GetDisasmColor(disasmData.mnemonic.type);
		insData.mnemonic = ImGuiCustomString(disasmData.mnemonic.value)(color);
	}
}

void ViewDisassembler::Disassemble()
{
	m_instructionList.clear();

	if (!Engine::IsAttached()) return;

	POINTER startAddress = m_pVirtualBase - 0x20;
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
				DisassembleRegion(regionIter->start, m_memoryBuffer.data() + regionIter->start - startAddress, regionIter->size);

				offset += regionIter->size;
				if (++regionIter == regions.end()) break;
			}
		}

		// there are more bytes left after disassembling valid regions
		// push invalid instructions
		for (; offset < startAddress + bufferSize; offset++)
		{
			auto& instruction = m_instructionList.emplace_back();
			instruction.address = offset;
			instruction.isNotReadable = true;
		}
	}

	// find the start index of the address (skip the garbage instructions before it as we -0x10 to the address)
	for (m_instructionOffset = 0; m_instructionOffset < m_instructionList.size(); m_instructionOffset++)
	{
		if (m_instructionList[m_instructionOffset].address > m_pVirtualBase)
		{
			m_instructionOffset--;
			break;
		}
	}

	AnalyzeCrossReference();
}

void ViewDisassembler::GoToAddress(POINTER address)
{
	m_pVirtualBase = address;
	Disassemble();
}