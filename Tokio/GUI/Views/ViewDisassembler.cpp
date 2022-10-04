#pragma once
#include "stdafx.h"
#include "ViewDisassembler.h"

#include "imgui_custom.hpp"
#include "GUI/Widgets/Widgets.hpp"

#include "Settings.h"
#include "Engine/Engine.h"

// Table Callback
Widgets::Table::Execution
ViewDisassembler::TableRenderCallback(Widgets::Table* table, size_t index, void* UserData)
{
	ViewDisassembler* pThis = static_cast<ViewDisassembler*>(UserData);
	auto& insData = pThis->GetInstructionAt(index);
	ImVec2 cursorOffset(0.f, 0.f);
	//ImVec2 cursorPos = ImGui::GetCursorPos();

	//ImGuiTable* im_table = table->GetHandle();
	//// stop rendering if we are out of sight on the table
	//if (cursorPos.y > im_table->InnerWindow->Size.y)
	//{
	//	if (insData.cursorPos.x != 0 && insData.cursorPos.y != 0)
	//	{
	//		insData.isRendered = false;
	//		return Widgets::Table::Execution::Skip;
	//	}
	//}

	if (insData.isBaseOffset)
	{
		cursorOffset.y += 25.f;
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 10.f));
		ImGui::PushFont(MainApplication::FontMonoBigBold);
		ImGui::TextUnformatted(insData.addressSymbol.c_str(), insData.addressSymbol.c_str() + insData.addressSymbol.size());
		ImGui::PopFont();
	}
	else if (insData.refererIndexes.size() != 0)
	{
		auto& refererIns = pThis->m_instructionList[insData.refererIndexes[0]];
		if (refererIns.mnemonic_type == DisasmOperandType::mneJump ||
			refererIns.mnemonic_type == DisasmOperandType::mneJumpCondition)
		{
			cursorOffset.y += 15.f;
			ImGui::Text("loc_%llx", insData.address);
		}
	}

	// draw symbolic address
	if (insData.addressSymbol.size() > 0 )
	{
		ImGui::TextUnformatted(insData.addressSymbol.c_str(), insData.addressSymbol.c_str() + insData.addressSymbol.size());
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Text, Settings::theme.disasm.Address);
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
		static std::string notReadableColored = "??"_c(Settings::theme.disasm.Invalid);
		ImGui::TextUnformatted(notReadableColored.c_str(), notReadableColored.c_str() + notReadableColored.size());
	}
	else
	{

		for (size_t i = 0; i < insData.length; i++)
		{
			ImGui::Text("%02X", pThis->m_memoryBuffer[insData.bufferOffset + i]);
			//ImGui::TextColored(ImVec4(ThemeSettings::disasm.Bytes), "%02X", insData.bytes[i]);
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

	//insData.isRendered = true;

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


	Settings::shortcuts.DisasmCopyAddress.RenderInPopup();

	ImGui::Separator();
	Settings::shortcuts.DisasmGoToAdress.RenderInPopup();

	ImGui::Separator();
	Settings::shortcuts.DisasmFollowInstruction.RenderInPopup();
	Settings::shortcuts.DisasmGoToReference.RenderInPopup();

	ImGui::Separator();
	Settings::shortcuts.DisasmAddToWatchList.RenderInPopup();
	Settings::shortcuts.DisasmOpenInMemoryView.RenderInPopup();

}

void ViewDisassembler::PopupNavigateRenderCallback(Widgets::Popup* popup, void* OpenUserData, void* UserData)
{
	ViewDisassembler* pThis = static_cast<ViewDisassembler*>(UserData);

	ImGui::SetNextItemWidth(-1);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 5.f, 7.f });
	bool isEnter = pThis->m_popupNavigateTextInput.Render();
	ImGui::PopStyleVar();
	
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 cursorPos = ImGui::GetCursorPos();
	ImVec2 btnSize = { 100, 25 };
	ImGui::SetCursorPos({ (windowSize.x - btnSize.x) / 2.f, cursorPos.y + 5.f });
	
	isEnter |= ImGui::Button("Ok", btnSize);

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

void ViewDisassembler::RenderReferenceArrow()
{
	auto* dl = ImGui::GetWindowDrawList();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 heightOffset{ -5.f, ImGui::GetTextLineHeight() / 2.f };


	ImGuiTable* table = m_table.GetHandle();
	ImDrawList* draw_list = table->InnerWindow->DrawList;
	draw_list->PushClipRect(table->InnerClipRect.Min + ImVec2(0, table->RowPosY2 - table->RowPosY1 + 2), table->InnerClipRect.Max);


	const auto RenderReference = [=](const ViewInstructionData& insData, ImU32 color, float width = 1.f, float triangleSize = 3.f)
	{
		static ImVec2 polyline[4];
		static ImVec2 polyline2[3];

		auto& refA = insData;
		auto& refB = m_instructionList[refA.referenceIndex];

		float depthLevel = refA.refDepthLevel + 1.f;

		polyline[0] = windowPos + refA.cursorPos + heightOffset;
		polyline[3] = windowPos + refB.cursorPos + heightOffset;

		// for an instruction that is index < m_instructionOffset, it wasn't being drawn so it doesn't have the cursor offset
		if (refA.cursorPos.y == 0.f)
			polyline[0] = { polyline[3].x, windowPos.y };
		else if (refB.cursorPos.y == 0.f)
			polyline[3] = { polyline[0].x, windowPos.y };

		polyline[1] = polyline[0] - ImVec2(5.f * depthLevel, 0.f);
		polyline[2] = polyline[3] - ImVec2(5.f * depthLevel, 0.f);

		// triangle arrow
		ImVec2 tria1 = polyline[3] + ImVec2(3 - triangleSize, -triangleSize);
		ImVec2 tria2 = polyline[3] + ImVec2(3, 0);
		ImVec2 tria3 = polyline[3] + ImVec2(3 - triangleSize, triangleSize);


		table->InnerWindow->DrawList->AddPolyline(polyline, 4, color, ImDrawFlags_None, width);
		table->InnerWindow->DrawList->AddTriangleFilled(tria1, tria2, tria3, color);
	};

	const auto RenderHighLight = [=](const ViewInstructionData& insData)
	{
		// render highlight if it has a reference
		if (insData.referenceIndex != UPTR_UNDEFINED)
			RenderReference(insData, Settings::theme.disasm.ReferenceHighLight, 2.f, 5.f);

		for (auto& referer : insData.refererIndexes)
		{
			RenderReference(m_instructionList[referer], Settings::theme.disasm.ReferenceHighLight, 2.f, 5.f);
		}
	};

	for (auto it = m_referenceList.begin(); it != m_referenceList.end(); it++)
	{
		auto& insData = it->get();
		ImU32 color = Settings::GetDisasmColor(insData.mnemonic_type);
		RenderReference(insData, color);
	}

	if (size_t hoveredIndex = m_table.GetHoveredIndex(); hoveredIndex != UPTR_UNDEFINED)
	{
		auto& insData = GetInstructionAt(hoveredIndex);
		RenderHighLight(insData);
	}

	if (auto& selectedIndex = m_table.GetSelectedItems(); selectedIndex.size() > 0)
	{
		auto& insData = GetInstructionAt(selectedIndex[0]);
		RenderHighLight(insData);
	}

	draw_list->PopClipRect();

}

// FIXME: THIS CODE IS SO BAD
int ViewDisassembler::RenderScrollBar()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiTable* im_table = m_table.GetHandle();
	ImDrawList* draw_list = im_table->InnerWindow->DrawList;

	ImRect windowRect = im_table->InnerClipRect;

	ImRect scrollBarRect{
		windowRect.Max.x - style.ScrollbarSize, windowRect.Min.y,
		windowRect.Max.x, windowRect.Max.y
	};

	float scrollHeight = windowRect.Max.y - windowRect.Min.y;
	float grabSize = scrollHeight / 8.f;
	int scrollDir = 0;

	ImRect grabRect{
		scrollBarRect.Min.x + style.FramePadding.y, windowRect.Min.y + (scrollHeight - grabSize) / 2.f,
		scrollBarRect.Max.x - style.FramePadding.y, windowRect.Min.y + (scrollHeight + grabSize) / 2.f
	};

	ImU32 colorBg = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_ScrollbarBg]);
	ImU32 colorGrab = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_ScrollbarGrab]);

	draw_list->AddRectFilled(scrollBarRect.Min, scrollBarRect.Max, colorBg);

	ImGui::SetCursorScreenPos(grabRect.Min);
	ImGui::InvisibleButton("##invisible_disasm_scrollbar", grabRect.Max - grabRect.Min);

	if (!m_scrollBarDragging)
	{
		if (ImGui::IsItemHovered())
		{
			bool bMouseDown = ImGui::IsMouseDown(0);
			colorGrab = ImGui::ColorConvertFloat4ToU32(
				style.Colors[bMouseDown ? ImGuiCol_ScrollbarGrabActive : ImGuiCol_ScrollbarGrabHovered]
			);
			if (bMouseDown && ImGui::IsMouseDragging(0)) m_scrollBarDragging = true;
		}

		draw_list->AddRectFilled(grabRect.Min, grabRect.Max, colorGrab, style.ScrollbarRounding);
	}
	else
	{
		if (!ImGui::IsMouseDown(0)) m_scrollBarDragging = false;

		colorGrab = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_ScrollbarGrabActive]);

		float dragDelta = ImGui::GetMouseDragDelta(0).y;

		grabRect.Min.y += dragDelta;
		grabRect.Max.y += dragDelta;

		if (grabRect.Min.y < scrollBarRect.Min.y)
		{
			grabRect.Min.y = scrollBarRect.Min.y;
			grabRect.Max.y = scrollBarRect.Min.y + grabSize;
			dragDelta = -scrollHeight / 2.f;
		}
		else if (grabRect.Max.y > scrollBarRect.Max.y)
		{
			grabRect.Min.y = scrollBarRect.Max.y - grabSize;
			grabRect.Max.y = scrollBarRect.Max.y;
			dragDelta = scrollHeight / 2.f;
		}

		draw_list->AddRectFilled(grabRect.Min, grabRect.Max, colorGrab, style.ScrollbarRounding);


		double currentTime = ImGui::GetTime();

		// normalize the drag delta
		dragDelta /= (scrollHeight / 2.f);
		double timeInterval = abs(dragDelta);
		if (currentTime - m_scrollDragInterval > 0.001f / timeInterval)
		{
			m_scrollDragInterval = currentTime;
			scrollDir = dragDelta > 0.f ? 1 : -1;
		}
	}

	return scrollDir;
}

void ViewDisassembler::HandleScrolling()
{
	int scrollDir = RenderScrollBar();

	// scroll by mouse
	if (scrollDir == 0 && ImGui::IsWindowHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_ChildWindows))
	{
		auto& io = ImGui::GetIO();
		if (io.MouseWheel != 0.f)
		{
			scrollDir = io.MouseWheel > 0.f ? -1 : 1;
		}
	}

	if (scrollDir != 0)
	{

		m_instructionOffset += scrollDir;
		m_pVirtualBase = m_instructionList[m_instructionOffset].address;

		Disassemble();

		if (auto& selectedItems = m_table.GetSelectedItems(); selectedItems.size() > 0)
		{
			// TODO: When scrolling down, if the selected index is == 0 we couldn't "hide" it and return
			// it back when scrolling back up, please handle this ugliness
			if (selectedItems[0] > 0 && selectedItems[0] < m_instructionList.size() - m_instructionOffset - 1)
				m_table.AddSelectedItem(selectedItems[0] - scrollDir);
		}

	}

}

void ViewDisassembler::HandleShortcuts()
{
	if (Settings::shortcuts.DisasmGoToAdress.IsPressedInWindow())
	{
		m_popupNavigate.Open();
		m_popupNavigateTextInput.Focus();
		auto& selectedItems = m_table.GetSelectedItems();
		if (selectedItems.size() > 0)
		{
			// TODO: HANDLE GO TO ADDRESS
		}
	}

	if (Settings::shortcuts.DisasmCopyAddress.IsPressedInWindow())
	{
		auto& selectedItems = m_table.GetSelectedItems();
		if (selectedItems.size() > 0)
		{
			if (auto result = common::BhClipboardCopy(GetInstructionAt(selectedItems[0]).address); result.has_error())
				result.error().show();
		}
	}

	if (Settings::shortcuts.DisasmFollowInstruction.IsPressedInWindow())
	{
		if (auto& selected = m_table.GetSelectedItems(); selected.size() > 0)
		{
			auto& insData = GetInstructionAt(selected[0]);
			POINTER refAddress = insData.refAddress;
			POINTER refValue = insData.refAddress;

			if (refAddress || refValue)
			{
				GoToAddress((refValue != 0) ? refValue : refAddress);
			}
		}
	}
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

	RenderReferenceArrow();

	// must handle shortcuts before end window
	HandleShortcuts();
	HandleScrolling();

	ImGui::End();

	m_popupNavigate.Render();

}


void ViewDisassembler::OnAttach(const std::shared_ptr<ProcessData>& targetProcess)
{
	m_SymbolHandler = Engine::Symbol();
	GoToAddress(targetProcess->baseModule->base);
}

void ViewDisassembler::OnDetach()
{
	m_referenceList.clear();
	m_instructionList.clear();
	m_SymbolHandler.reset();
}

std::string FormatSymbolAddress(
	POINTER address,
	const ResultGetSymbol& resultGetModuleSymbol,
	bool* isBaseOffset = nullptr)
{
	static char symbol[1024];
	static std::string regularAddress = "%llX"_c(Settings::theme.disasm.AddressAbs);

	static std::string formatProcModBase = 
		"%s"_c(Settings::theme.disasm.Module);

	static std::string formatProcMod = 
		"%s"_c(Settings::theme.disasm.Module) +
		"+"_c(Settings::theme.disasm.Delimeter) +
		"%llX"_c(Settings::theme.disasm.Displacement);

	static std::string formatFunc = 
		"%s"_c(Settings::theme.disasm.Module) 
		+ "."_c(Settings::theme.disasm.Delimeter)
		+ "%s"_c(Settings::theme.disasm.Function)
		+ "+"_c(Settings::theme.disasm.Delimeter) 
		+ "%llX"_c(Settings::theme.disasm.Displacement);

	static std::string formatFuncBase = 
		"%s"_c(Settings::theme.disasm.Module) +
		"."_c(Settings::theme.disasm.Delimeter) +
		"%s"_c(Settings::theme.disasm.Function);

	// parse the address normally
	if (!resultGetModuleSymbol.has_value())
	{
		_snprintf_s(symbol, sizeof(symbol), regularAddress.c_str(), address);
		return symbol;
	}

	if (!resultGetModuleSymbol.has_symbol())
	{
		auto pModule = resultGetModuleSymbol.Module();
		if (address != pModule->base)
		{
			_snprintf_s(symbol, sizeof(symbol), formatProcMod.c_str(), pModule->moduleNameA.c_str(), address - pModule->base);
			if (isBaseOffset) *isBaseOffset = false;
		}
		else
		{
			_snprintf_s(symbol, sizeof(symbol), formatProcModBase.c_str(), pModule->moduleNameA.c_str(), address - pModule->base);
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
			_snprintf_s(symbol, sizeof(symbol), formatFunc.c_str(), modShortName.c_str(), pSymbol->name.c_str(), offsetFromVA);
			if (isBaseOffset) *isBaseOffset = false;
		}
		else
		{
			_snprintf_s(symbol, sizeof(symbol), formatFuncBase.c_str(), modShortName.c_str(), pSymbol->name.c_str());
			if (isBaseOffset) *isBaseOffset = true;
		}
	}

	return symbol;
}


// analyze the reference of the instruction (if it is a pointer e.g isRefPointer == true)
void ViewDisassembler::AnalyzeInstructionReference(ViewInstructionData& insData)
{
	static std::string formatString = "\"%s\""_c(Settings::theme.disasm.String);
	static std::string formatPointer = "[%llX]"_c(Settings::theme.disasm.AddressAbs);
	static std::string formatDecimal = "%d"_c(Settings::theme.disasm.Displacement);
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
				_snprintf_s(bufferFmt, sizeof(bufferFmt), formatString.c_str(), stringBuffer);

				// strip \n out of the comment
				std::string comment(bufferFmt);
				if (auto find = comment.rfind('\n'); find != std::string::npos)
					comment = comment.substr();

				insData.comment = comment;
				return;
			}
			else if (isValidWString)
			{
				std::wstring wideString(reinterpret_cast<wchar_t*>(stringBuffer), 32);
				_snprintf_s(bufferFmt, sizeof(bufferFmt), formatString.c_str(), common::BhString(wideString).c_str());

				// strip \n out of the comment
				std::string comment(bufferFmt);
				if (auto find = comment.rfind('\n'); find != std::string::npos)
					comment = comment.substr();

				insData.comment = comment;
				return;
			}
		}

		// if it's not a string, then might it be a pointer?
		resultRead = Engine::ReadMem<POINTER>(refPointer);
		if (!resultRead.has_error())
		{
			_snprintf_s(bufferFmt, sizeof(bufferFmt), formatPointer.c_str(), refPointer);
			insData.comment = bufferFmt;
			return;
		}

		// it it's not anything above, just format it as a decimal value
		_snprintf_s(bufferFmt, sizeof(bufferFmt), formatDecimal.c_str(), refPointer);
		insData.comment = bufferFmt;
	}
}

void ViewDisassembler::AnalyzeCrossReference()
{
	static char bufferFmt[1024];
	std::string formatXRef = "XREF: %s "_c(Settings::theme.disasm.Xref);

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
			bool bHasReference = false;
			if (it->refAddress > it->address)
			{
				reference = std::find(it, m_instructionList.end(), it->refAddress);
				bHasReference = reference != m_instructionList.end();
			}
			else
			{
				reference = std::find(m_instructionList.begin(), it, it->refAddress);
				bHasReference = reference != it;
			}

			if (bHasReference)
			{
				it->referenceIndex = reference - m_instructionList.begin();
				reference->refererIndexes.push_back(it - m_instructionList.begin());
				//reference->refererAddress = it->address;

				_snprintf_s(bufferFmt, sizeof(bufferFmt), formatXRef.c_str(), it->addressSymbol.c_str());

				reference->comment += bufferFmt;

				m_referenceList.push_back(*it);
			}
			else
			{
				it->refAddress = 0;
				it->referenceIndex = UPTR_UNDEFINED;
				it->isRefPointer = false;
			}


		}
	}

	// calculate the "depth" level of the arrow line
	for (auto it = m_referenceList.begin(); it != m_referenceList.end(); it++)
	{
		auto& refA = it->get();

		POINTER addrStartA, addrEndA;
		if (refA.address > refA.refAddress)
		{
			addrStartA = refA.refAddress;
			addrEndA = refA.address;
		}
		else
		{
			addrStartA = refA.address;
			addrEndA = refA.refAddress;
		}

		for (auto jt = m_referenceList.begin(); jt != m_referenceList.end(); jt++)
		{
			auto& refB = jt->get();
			POINTER addrStartB, addrEndB;

			if (refB.address > refB.refAddress)
			{
				addrStartB = refB.refAddress;
				addrEndB = refB.address;
			}
			else
			{
				addrStartB = refB.address;
				addrEndB = refB.refAddress;
			}

			// refB cover refA
			bool case1 = addrStartB < addrStartA && addrEndB    > addrEndA;

			// refB inside refA
			bool case2 = addrStartB >  addrStartA && addrEndB    < addrEndA;

			// refB start inside refA and end outside
			bool case3 = addrStartB < addrStartA && addrEndB    > addrStartA && addrEndB < addrEndA;

			// refB start outside refA and end inside
			bool case4 = addrStartB >  addrStartA && addrStartB  < addrEndA  && addrEndB >  addrEndA;

			bool case5 = addrStartB == addrStartA;
			bool case6 = addrEndB == addrEndA;
			bool case7 = refB.refDepthLevel == refA.refDepthLevel;

			if (case1 && case7) refB.refDepthLevel += 1.f;
			if (case2 && case7) refB.refDepthLevel += 1.f;
			//if (case3 && case7) refB.refDepthLevel += 1.f;
			//if (case4 && case7) refB.refDepthLevel += 1.f;

			//if (case5) refB.refDepthLevel += 1.f;
			if (case5)
			{
				if (addrEndB > addrEndA)
				{
					//if (refB.refDepthLevel == 0.f || refB.refDepthLevel == refA.refDepthLevel)
						refA.refDepthLevel += 1.f;
					//refB.refDepthLevel += 1.f;
				}
				else
				{
					//if (refA.refDepthLevel == 0.f || refB.refDepthLevel == refA.refDepthLevel)
				}
			}
			if (case6)
			{
				if (addrStartB > addrStartA)
				{
					//if (refB.refDepthLevel == 0.f || refB.refDepthLevel == refA.refDepthLevel)
					refA.refDepthLevel += 1.f;
					//refB.refDepthLevel += 1.f;
				}
				else
				{
					//if (refA.refDepthLevel == 0.f || refB.refDepthLevel == refA.refDepthLevel)
					//refB.refDepthLevel += 1.f;
				}
			}

		}
	}
}

void ViewDisassembler::DisassembleRegion(const MemoryRegion& region, size_t bufferOffset)
{
	auto disasmResult = Engine::Disassembler()->Disasm(region.start, m_memoryBuffer.data() + bufferOffset, region.size);
	assert(disasmResult.has_value() && "Disassemble failed");

	auto& instructions = disasmResult.value();

	size_t currentInstructionIndex = m_instructionList.size();
	size_t instructionIndex = 0;
	size_t instructionOffset = bufferOffset;
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

		insData.bufferOffset = instructionOffset;

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
				DWORD color = Settings::GetDisasmColor(operand.type);
				insData.instruction += ImGuiCustomString(operand.value)(color);
			}
		}

		// if it's is something like `mov rax, [0x12345]`
		if (insData.isRefPointer) AnalyzeInstructionReference(insData);

		DWORD color = Settings::GetDisasmColor(disasmData.mnemonic.type);
		insData.mnemonic = ImGuiCustomString(disasmData.mnemonic.value)(color);

		instructionOffset += insData.length;
	}
}

void ViewDisassembler::Disassemble()
{
	m_referenceList.clear();
	m_instructionList.clear();

	if (!Engine::IsAttached()) return;

	POINTER startAddress = m_pVirtualBase - 32;
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
				DisassembleRegion(*regionIter, regionIter->start - startAddress);

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