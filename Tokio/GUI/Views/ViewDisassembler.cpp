#pragma once
#include "stdafx.h"
#include "ViewDisassembler.h"

#include "GUI/Widgets/Widgets.hpp"
#include "Resources/FontAwesomeImpl.h"

#include "Engine/Engine.h"

// Table Callback
Widgets::Table::Execution
ViewDisassembler::TableRenderCallback(Widgets::Table* table, size_t index, void* UserData)
{
	ViewDisassembler* pThis = static_cast<ViewDisassembler*>(UserData);
	auto& insData = pThis->m_Instructions[index];

	ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextAddressModule), "%llX", insData.address);

	table->NextColumn();

	ImGui::Text("%s", insData.instruction.c_str());

	return Widgets::Table::Execution::Continue;
};

void ViewDisassembler::TableInputCallback(Widgets::Table* table, size_t index, void* UserData)
{
	ViewDisassembler* pThis = static_cast<ViewDisassembler*>(UserData);

	// if any row is hovered
	if (index != UPTR_UNDEFINED)
	{

	}
}

void ViewDisassembler::TablePopupRenderCallback(Widgets::Table* table, size_t index, void* UserData)
{
	ViewDisassembler* pThis = static_cast<ViewDisassembler*>(UserData);

	// popup events are guaranteed to have at least one selected item
	index = table->GetSelectedItems()[0];


	bool bCopyAddress    = ImGui::MenuItem(ICON_FA_CLIPBOARD u8" Copy address",        "Ctrl+C");
	bool bGotoAddress    = ImGui::MenuItem(                  u8"⏩ Go to address",      "Ctrl+G");
	bool bOpenInMemory   = ImGui::MenuItem(ICON_MEMORY_VIEW  u8" Open in Memory View", "Ctrl+B");
	bool bAddToWatchList = ImGui::MenuItem(ICON_WATCH_LIST   u8" Add to Watch List",   "Ctrl+E");

	if (bCopyAddress) 
		if (auto result = common::BhClipboardCopy(pThis->m_Instructions[index].address); result.has_error()) 
			result.error().show();
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
		ImGuiTableFlags_ScrollY | // enable vertical scrool
		ImGuiTableFlags_Hideable | // hide the columns
		ImGuiTableFlags_Resizable | // resizable coulmns by default
		ImGuiTableFlags_Reorderable; // re-order the coulmns

	m_table.Setup(desc);

	m_table.AddColumn("Address", ImGuiTableColumnFlags_NoHide);
	m_table.AddColumn("Bytes");
	m_table.AddColumn("Opcode");
	m_table.AddColumn("Comments");

}

void ViewDisassembler::Render(bool& bOpen)
{
	if (!bOpen) return;

	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 0,0 });
	ImGui::Begin(Title().c_str(), &bOpen);
	ImGui::PopStyleVar();

	ImGui::PushFont(MainApplication::FontMonoRegular);
	m_table.Render(m_Instructions.size());
	ImGui::PopFont();
	ImGui::End();
}


void ViewDisassembler::Update(const std::shared_ptr<ProcessData>& targetProcess)
{
	GoToAddress(targetProcess->base().address);
}

void ViewDisassembler::GoToAddress(POINTER address)
{
	m_pVirtualBase = address;

	BYTE pBuffer[1024];
	auto readMemResult = Engine::ReadMem(m_pVirtualBase, pBuffer, sizeof(pBuffer));
	if (readMemResult.has_error())
	{
		readMemResult.error().show(L"Disassembler failed to read to memory process");
		return;
	}

	auto disasmResult = Engine::Disassembler()->Disasm(m_pVirtualBase, pBuffer, sizeof(pBuffer));
	if (disasmResult.has_error())
	{
		disasmResult.error().show(L"Error while disassembling the target process");
	}
	else
	{
		m_Instructions = disasmResult.value();
		m_table.AddSelectedItem(0);
	}
}

