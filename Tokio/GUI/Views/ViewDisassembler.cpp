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
	auto& insData = pThis->m_disasmData[index];
	ImVec2 cursorOffset(0.f, 0.f);

	if (insData.isBaseOffset)
	{
		cursorOffset.y += 25.f;
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 10.f));
		ImGui::PushFont(MainApplication::FontMonoBigBold);
		ImGui::Text("%s", insData.addressSymbol.c_str());
		ImGui::PopFont();
	}

	ImGui::Text("%s", insData.addressSymbol.c_str());


	// bytes
	table->NextColumn();
	ImGui::SetCursorPos(ImGui::GetCursorPos() + cursorOffset);

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

	ImGui::Text("%s", insData.mnemonic.c_str());

	ImGui::SetCursorPos(cursorPos + ImVec2(55.f, 0.f));
	ImGui::Text("%s", insData.instruction.c_str());

	if (insData.comment.size() > 0)
	{
		table->NextColumn();
		ImGui::Text("%s", insData.comment.c_str());
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
			POINTER refAddress = pThis->m_disasmData[selected[0]].refAddress;
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

	desc.Flags = ImGuiTableFlags_BordersOuter | // outer borders
				 ImGuiTableFlags_ScrollY      | // enable vertical scrool
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
		GoToAddress(m_pVirtualBase);
	}

	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 0,0 });
	ImGui::Begin(Title().c_str(), &bOpen);
	ImGui::PopStyleVar();

	ImGui::PushFont(MainApplication::FontMonoRegular);
	m_table.Render(m_disasmData.size());
	ImGui::PopFont();
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
			if (auto result = common::BhClipboardCopy(m_disasmData[selectedItems[0]].address); result.has_error())
				result.error().show();
		}

		m_shortcutData.bCopyAddress = false;
	}
}


void ViewDisassembler::Update(const std::shared_ptr<ProcessData>& targetProcess)
{
	GoToAddress(targetProcess->baseModule->base);
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

// TODO: Fix this mess
void ViewDisassembler::GoToAddress(POINTER address)
{

	BYTE pBuffer[1024];
	auto readMemResult = Engine::ReadMem(address, pBuffer, sizeof(pBuffer));
	if (readMemResult.has_error())
	{
		//readMemResult.error().show(L"Disassembler failed to read memory process");
		//return;
	}

	auto disasmResult = Engine::Disassembler()->Disasm(address, pBuffer, sizeof(pBuffer));
	if (disasmResult.has_error())
	{
		disasmResult.error().show(L"Error while disassembling the target process");
	}
	else
	{
		m_disasmData.clear();
		m_disasmData.resize(disasmResult.value().size());

		size_t index = 0;

		auto symbolEngine = Engine::Symbol();
		auto walkContext = symbolEngine->AddressSymbolWalkInit();

		for (auto& disasmData : disasmResult.value())
		{
			auto resultGetModuleSymbol = symbolEngine->AddressSymbolWalkNext(walkContext, disasmData.address);

			auto& viewDisasmData = m_disasmData[index];
			viewDisasmData.address       = disasmData.address;
			viewDisasmData.addressSymbol = FormatSymbolAddress(disasmData.address, resultGetModuleSymbol, &viewDisasmData.isBaseOffset);
			viewDisasmData.refAddress    = disasmData.refAddress;
			viewDisasmData.isRefPointer  = disasmData.isRefPointer;
			viewDisasmData.length        = disasmData.length; 

			memcpy_s(viewDisasmData.bytes, sizeof(viewDisasmData.bytes), disasmData.bytes, sizeof(disasmData.bytes));

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
					auto resultGetModuleSymbol = symbolEngine->AddressToModuleSymbol(viewDisasmData.refAddress);
					viewDisasmData.instruction += FormatSymbolAddress(viewDisasmData.refAddress, resultGetModuleSymbol);
				}
				// it's just a regular operand
				else
				{
					DWORD color = ThemeSettings::GetDisasmColor(operand.type);
					viewDisasmData.instruction += ImGuiCustomString(operand.value)(color);
				}
			}

			if (viewDisasmData.isRefPointer)
			{
				auto resultRead = Engine::ReadMem<POINTER>(viewDisasmData.refAddress);
				if (resultRead.has_value())
				{
					POINTER refPointer = resultRead.value();
					auto resultGetModuleSymbol = symbolEngine->AddressToModuleSymbol(refPointer);
					viewDisasmData.comment = FormatSymbolAddress(refPointer, resultGetModuleSymbol);
				}
			}

			DWORD color = ThemeSettings::GetDisasmColor(disasmData.mnemonic.type);
			viewDisasmData.mnemonic = ImGuiCustomString(disasmData.mnemonic.value)(color);

			index++;
		}

		m_table.AddSelectedItem(0);
		m_table.SetScroll(0.f);

		m_pVirtualBase = address;
	}
}

