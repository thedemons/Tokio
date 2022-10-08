#pragma once
#include "stdafx.h"
#include "ViewDisassembler.h"
#include "ViewDecompiler.h"

#include "GUI/Widgets/Widgets.hpp"
#include "Engine/Engine.h"
#include "Common/SystemHelper.h"

#include "Settings.h"

using namespace std::string_literals;

// Table Callback
Widgets::Table::Execution
ViewDisassembler::TableRenderCallback(Widgets::Table* table, size_t index, void* UserData)
{
	ViewDisassembler* pThis = static_cast<ViewDisassembler*>(UserData);
	auto& insData = pThis->GetInstructionAt(index);
	ImVec2 cursorOffset(0.f, 0.f);

	static ImGui::TokenizedText notReadableColored(1);

	ImVec2 checkCursorPos = ImGui::GetCursorPos();

	ImGuiTable* im_table = table->GetHandle();
	// stop rendering if we are out of sight on the table
	if (checkCursorPos.y > im_table->InnerWindow->Size.y)
	{
		return Widgets::Table::Execution::Stop;
	}
	//im_table->Columns[1].ItemWidth

	// not readable address, draw a "??"
	if (insData.isNotReadable)
	{
		if (notReadableColored.size() == 0) notReadableColored.push_back(Settings::theme.disasm.Invalid, "??");

		ImGui::Text("%llX", insData.address);

		table->NextColumn();
		notReadableColored.Render();

		table->NextColumn();
		notReadableColored.Render();

		return Widgets::Table::Execution::Continue;
	}

	// if the instruction is at the start at a module
	if (insData.isAtBaseModule)
	{
		cursorOffset.y += 25.f;
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 10.f));

		insData.fmtAddress.Render(MainApplication::FontMonoBigBold);
	}

	// draw symbolic address
	// FIXME: is checking the size needed?

	if (insData.fmtAddress.size() > 0)
	{
		insData.fmtAddress.Render();
	}
	else
	{
		ImVec4 addrColor = ImGui::ColorConvertU32ToFloat4(Settings::theme.disasm.AddressAbs);
		ImGui::TextColored(addrColor, "%llX", insData.address);
	}

	const SubroutineInfo* subroutine = insData.GetSubroutine();

	if (subroutine != nullptr && subroutine->address == insData.address)
	{
		if (insData.isAtSubroutineStart)
		{
			ImVec4 subColor = ImGui::ColorConvertU32ToFloat4(Settings::theme.disasm.Subroutine);

			ImGui::SameLine();
			ImGui::TextColored(subColor, "sub_%llx start %d", insData.address, insData.iSubroutine);

			insData.fmtAddress.Render();
			ImGui::SameLine();
			ImGui::TextColored(subColor, "blocks: %d", subroutine->blocks.size());

			insData.fmtAddress.Render();
			ImGui::SameLine();
			ImGui::TextColored(subColor, "size: 0x%llX", subroutine->size);


			cursorOffset.y += 17.f*2.f;
		}
		else if (insData.referers.size() != 0)
		{
			auto refererIns = insData.referers[0];
			if (refererIns->mnemonic.type == DisasmOperandType::mneJump ||
				refererIns->mnemonic.type == DisasmOperandType::mneJumpCondition)
			{
				ImVec4 locColor = ImGui::ColorConvertU32ToFloat4(Settings::theme.disasm.mneJumpCondition);

				ImGui::SameLine();
				ImGui::TextColored(locColor, "loc_%llx", insData.address);
				insData.fmtAddress.Render();

				cursorOffset.y += 17.f;
			}
		}
	}


	//assert((!insData.isAtSubroutineEnd || (insData.isAtSubroutineEnd && subroutine != nullptr)) && "Why is it at the end of the subroutine but doesn't have a pointer to any subroutine?");
	if (insData.isAtSubroutineEnd && subroutine)
	{
		insData.fmtAddress.Render();
		ImGui::SameLine();

		ImVec4 subColor = ImGui::ColorConvertU32ToFloat4(0xff00ff00);
		ImGui::TextColored(subColor, "sub_%llx endp", subroutine->address);
	}
	
	// bytes column
	table->NextColumn();
	ImGui::SetCursorPos(ImGui::GetCursorPos() + cursorOffset);


	ImGui::PushStyleColor(ImGuiCol_Text, Settings::theme.disasm.Bytes);
	for (size_t i = 0; i < insData.length; i++)
	{
		ImGui::Text( "%02X", pThis->m_memoryBuffer[insData.bufferOffset + i]);
		ImGui::SameLine();
	}
	ImGui::PopStyleColor();


	// mnemonic
	table->NextColumn();

	ImVec2 cursorPos = ImGui::GetCursorPos() + cursorOffset;
	ImGui::SetCursorPos(cursorPos);

	insData.fmtMnemonic.Render();

	// instructions
	ImGui::SetCursorPos(cursorPos + ImVec2(55.f, 0.f));
	insData.fmtOperand.Render();

	// comment
	if (insData.fmtComment.size() > 0)
	{
		table->NextColumn();
		insData.fmtComment.Render();
	}
	return Widgets::Table::Execution::Continue;
};

void ViewDisassembler::TableInputCallback(Widgets::Table* table, size_t index, void* UserData)
{
	UNUSED(table);
	UNUSED(index);
	UNUSED(UserData);

	//ViewDisassembler* pThis = static_cast<ViewDisassembler*>(UserData);

	// if any row is hovered
	if (index != UPTR_UNDEFINED)
	{

	}

}

void ViewDisassembler::TablePopupRenderCallback(Widgets::Table* table, size_t index, void* UserData)
{
	UNUSED(UserData);

	// popup events are guaranteed to have at least one selected item
	index = table->GetSelectedItems()[0];


	Settings::shortcuts.DisasmGoToAdress.RenderInPopup();
	Settings::shortcuts.DisasmCopyAddress.RenderInPopup();
	ImGui::Separator();

	Settings::shortcuts.DisasmFollowInstruction.RenderInPopup();
	Settings::shortcuts.DisasmGoToReference.RenderInPopup();

	ImGui::Separator();
	Settings::shortcuts.DisasmAddToWatchList.RenderInPopup();
	Settings::shortcuts.DisasmOpenInMemoryView.RenderInPopup();

}

void ViewDisassembler::PopupNavigateRenderCallback(Widgets::Popup* popup, void* OpenUserData, void* UserData)
{
	UNUSED(OpenUserData);

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
		POINTER address = Engine::Symbol()->SymbolToAddress(pThis->m_popupNavigateTextInput.str_strip());
		popup->Close();
		pThis->GoToAddress(address);
		pThis->m_popupNavigateTextInput.Clear();
	}

}

ViewDisassembler::ViewDisassembler()
{
	m_title = MainView::GetViewTitle<ViewDisassembler>(ICON_DISASSEMBLER u8" Disassembler");

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
				 //ImGuiTableFlags_Hideable     | // hide the columns
				 ImGuiTableFlags_Resizable    | // resizable coulmns by default
				 ImGuiTableFlags_Reorderable  ; // re-order the coulmns

	m_table.Setup(desc);

	m_table.AddColumn("Address");
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
	//ImVec2 windowPos = ImGui::GetWindowPos();
	//ImVec2 heightOffset{ -5.f, ImGui::GetTextLineHeight() / 2.f };


	//ImGuiTable* table = m_table.GetHandle();
	//ImDrawList* draw_list = table->InnerWindow->DrawList;
	//draw_list->PushClipRect(table->InnerClipRect.Min + ImVec2(0, table->RowPosY2 - table->RowPosY1 + 2), table->InnerClipRect.Max);


	//const auto RenderReference = [=](const ViewInstructionData& insData, ImU32 color, float width = 1.f, float triangleSize = 3.f)
	//{
	//	static ImVec2 polyline[4];
	//	static ImVec2 polyline2[3];

	//	auto& refA = insData;
	//	auto& refB = m_instructionList[refA.referenceIndex];

	//	float depthLevel = refA.refDepthLevel + 1.f;

	//	polyline[0] = windowPos + refA.cursorPos + heightOffset;
	//	polyline[3] = windowPos + refB.cursorPos + heightOffset;

	//	// for an instruction that is index < m_instructionOffset, it wasn't being drawn so it doesn't have the cursor offset
	//	if (refA.cursorPos.y == 0.f)
	//		polyline[0] = { polyline[3].x, windowPos.y };
	//	else if (refB.cursorPos.y == 0.f)
	//		polyline[3] = { polyline[0].x, windowPos.y };

	//	polyline[1] = polyline[0] - ImVec2(5.f * depthLevel, 0.f);
	//	polyline[2] = polyline[3] - ImVec2(5.f * depthLevel, 0.f);

	//	// triangle arrow
	//	ImVec2 tria1 = polyline[3] + ImVec2(3 - triangleSize, -triangleSize);
	//	ImVec2 tria2 = polyline[3] + ImVec2(3, 0);
	//	ImVec2 tria3 = polyline[3] + ImVec2(3 - triangleSize, triangleSize);


	//	table->InnerWindow->DrawList->AddPolyline(polyline, 4, color, ImDrawFlags_None, width);
	//	table->InnerWindow->DrawList->AddTriangleFilled(tria1, tria2, tria3, color);
	//};

	//const auto RenderHighLight = [=](const ViewInstructionData& insData)
	//{
	//	// render highlight if it has a reference
	//	if (insData.referenceIndex != UPTR_UNDEFINED)
	//		RenderReference(insData, Settings::theme.disasm.ReferenceHighLight, 2.f, 5.f);

	//	for (auto& referer : insData.refererIndexes)
	//	{
	//		RenderReference(m_instructionList[referer], Settings::theme.disasm.ReferenceHighLight, 2.f, 5.f);
	//	}
	//};

	//for (auto it = m_referenceList.begin(); it != m_referenceList.end(); it++)
	//{
	//	auto& insData = it->get();
	//	ImU32 color = Settings::GetDisasmColor(insData.mnemonic_type);
	//	RenderReference(insData, color);
	//}

	//if (size_t hoveredIndex = m_table.GetHoveredIndex(); hoveredIndex != UPTR_UNDEFINED)
	//{
	//	auto& insData = GetInstructionAt(hoveredIndex);
	//	RenderHighLight(insData);
	//}

	//if (auto& selectedIndex = m_table.GetSelectedItems(); selectedIndex.size() > 0)
	//{
	//	auto& insData = GetInstructionAt(selectedIndex[0]);
	//	RenderHighLight(insData);
	//}

	//draw_list->PopClipRect();
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

			if (bMouseDown && ImGui::IsMouseDragging(0))
			{
				m_scrollBarDragging = true;
				m_scrollDragInterval = ImGui::GetTime();
			}
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
		double timeInterval = 0.001f / abs(dragDelta);
		if (currentTime - m_scrollDragInterval > timeInterval)
		{
			double fdelta = (currentTime - m_scrollDragInterval) / timeInterval;
			m_scrollDragInterval = currentTime;

			int delta = static_cast<int>(round(fdelta));

			scrollDir = dragDelta > 0.f ? 1 * delta : -1 * delta;
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
			scrollDir = io.MouseWheel > 0.f ? -2 : 2;
		}
	}

	if (scrollDir != 0)
	{
		// not enough instructions in the list to goes backward, subtract the address instead
		if (scrollDir < 0 && m_instructionOffset < -scrollDir)
		{
			m_pVirtualBase += scrollDir;

			// we disassemble before the refresh rate hit because we ran out of instructions to display
			Disassemble();
		}
		else if(scrollDir > 0 && m_instructionOffset + scrollDir > m_analyzedData.instructions.size() - 1)
		{
			m_pVirtualBase += scrollDir;
			Disassemble();
		}
		else
		{
			m_instructionOffset += scrollDir;
			m_pVirtualBase = m_analyzedData.instructions[m_instructionOffset].address;
		}

		if (auto& selectedItems = m_table.GetSelectedItems(); selectedItems.size() > 0)
		{
			// TODO: When scrolling down, if the selected index is == 0 we couldn't "hide" it and return
			// it back when scrolling back up, please handle this ugliness
			size_t ins_list_size = m_analyzedData.instructions.size();
			size_t currentIndex = selectedItems[0];

			if (scrollDir > 0)
			{
				if (currentIndex >= scrollDir)
				{
					m_table.AddSelectedItem(currentIndex - scrollDir);
				}
				else
				{
					m_rememberedSelectedIndex = 1;
					m_table.ClearSelectedItems();
				}
			}
			else if (scrollDir < 0)
			{
				m_rememberedSelectedIndex = 0;

				currentIndex = (currentIndex - scrollDir <= ins_list_size - 1) ? currentIndex - scrollDir : ins_list_size - 1;
				m_table.AddSelectedItem(currentIndex);
			}
		}
		else
		{
			if (scrollDir > 0)
			{
				m_rememberedSelectedIndex += 1;
			}
			else if (scrollDir < 0)
			{
				m_rememberedSelectedIndex -= 1;

				if (m_rememberedSelectedIndex == 0)
					m_table.AddSelectedItem(0);
			}
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
			POINTER address = GetInstructionAt(selectedItems[0]).address;

			if (!Tokio::ClipboardCopy(address, true))
			{
				Tokio::Log("Copy address failed");
			}
		}
	}

	if (Settings::shortcuts.DisasmFollowInstruction.IsPressedInWindow())
	{
		if (auto& selected = m_table.GetSelectedItems(); selected.size() > 0)
		{
			auto& insData = GetInstructionAt(selected[0]);
			POINTER refAddress = insData.referencedAddress;
			POINTER refValue = insData.referencedAddress;

			if (refAddress || refValue)
			{
				GoToAddress((refValue != 0) ? refValue : refAddress);
			}
		}
	}

	if (Settings::shortcuts.DisasmSwitchMode.IsPressedInWindow())
	{
		auto& selected = m_table.GetSelectedItems();
		if (selected.size() > 0)
		{
			// do this to avoid nested hell
			const SubroutineInfo* pSubroutine = nullptr;

			POINTER selectedItemAddress = GetInstructionAt(selected[0]).address;
			for (const SubroutineInfo& subroutine : m_analyzedData.subroutines)
			{
				if (subroutine.address <= selectedItemAddress && selectedItemAddress <= subroutine.address + subroutine.size)
				{
					pSubroutine = &subroutine;
					break;
				}
			}

			if (pSubroutine != nullptr)
			{
				auto views = MainView::FindViewsByClass<ViewDecompiler>();
				views[0].get().pView->Decompile(pSubroutine->address, pSubroutine->size);
			}
		}
	}

	//if (Settings::shortcuts.DisasmSwitchMode.IsPressedInWindow())
	//{

	//	if (!m_isGraphMode)
	//	{
	//		auto& selected = m_table.GetSelectedItems();
	//		if (selected.size() > 0)
	//		{
	//			POINTER selectedItemAddress = GetInstructionAt(selected[0]).address;
	//			m_isGraphMode = m_graph.Init(selectedItemAddress, m_analyzedData);
	//		}
	//	}
	//	else
	//	{

	//		m_isGraphMode = false;
	//	}

	//}
}

void ViewDisassembler::Render(bool& bOpen)
{
	if (!bOpen) return;

	if (Engine::IsAttached())
	{
		double currentTime = ImGui::GetTime();
		if (currentTime - m_timeLastRefresh > m_refreshInterval)
		{
			m_timeLastRefresh = currentTime;
			Disassemble(Engine::AnalyzerFlags_::Symbol |
				Engine::AnalyzerFlags_::CrossReference |
				Engine::AnalyzerFlags_::Subroutine |
				Engine::AnalyzerFlags_::Comment);
		}
	}
	//GoToAddress(m_pVirtualBase);


	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 0.f, 0.f });
	ImGui::Begin(Title().c_str(), &bOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PopStyleVar();

	if (m_isGraphMode)
	{
		m_graph.Render();
	}
	else
	{
		m_table.Render(m_analyzedData.instructions.size() - m_instructionOffset);

		RenderReferenceArrow();
		HandleScrolling();
	}

	// must handle shortcuts before end window
	HandleShortcuts();

	ImGui::End();

	m_popupNavigate.Render();

}


void ViewDisassembler::OnAttach(const std::shared_ptr<ProcessData>& targetProcess)
{
	UNUSED(targetProcess);
	GoToAddress((POINTER)Settings::Load);

	//Engine::Analyzer()->FullAnalyze(nullptr, nullptr);

	//GoToAddress(targetProcess->baseModule->base);
	//GoToAddress(targetProcess->baseModule->base);

}

void ViewDisassembler::OnDetach()
{
	m_instructionOffset = 0;
	m_analyzedData.instructions.clear(); // FIXME
	m_analyzedData.subroutines.clear(); // FIXME
}

//
//// analyze the reference of the instruction (if it is a pointer e.g isRefPointer == true)

//
//void ViewDisassembler::AnalyzeCrossReference()
//{
//	static ImGui::TokenizedText xRefString("REF: "s, Settings::theme.disasm.Xref);
//
//	POINTER instructionStart = m_instructionList.front().address;
//	POINTER instructionEnd = m_instructionList.back().address;
//
//
//	// calculate the reference index, mainly used for jump and call pointer rendering
//	for (auto it = m_instructionList.begin(); it != m_instructionList.end(); it++)
//	{
//		// if the reference address is in range of the current instructions
//		if (it->refAddress != 0 &&
//			instructionStart <= it->refAddress &&
//			it->refAddress <= instructionEnd)
//		{
//
//			std::vector<ViewInstructionData>::iterator reference;
//
//			// we are optimizing the search by doing this in a sorted list
//			// will binary search be better?
//			bool bHasReference = false;
//			if (it->refAddress > it->address)
//			{
//				reference = std::find(it, m_instructionList.end(), it->refAddress);
//				bHasReference = reference != m_instructionList.end();
//			}
//			else
//			{
//				reference = std::find(m_instructionList.begin(), it, it->refAddress);
//				bHasReference = reference != it;
//			}
//
//			if (bHasReference)
//			{
//				it->referenceIndex = reference - m_instructionList.begin();
//				reference->refererIndexes.push_back(it - m_instructionList.begin());
//				//reference->refererAddress = it->address;
//
//				reference->comment += xRefString + it->addressSymbol;
//
//				m_referenceList.push_back(*it);
//			}
//			else
//			{
//				it->refAddress = 0;
//				it->referenceIndex = UPTR_UNDEFINED;
//				it->isRefPointer = false;
//			}
//
//
//		}
//	}
//
//	// calculate the "depth" level of the arrow line
//	for (auto it = m_referenceList.begin(); it != m_referenceList.end(); it++)
//	{
//		auto& refA = it->get();
//
//		POINTER addrStartA, addrEndA;
//		if (refA.address > refA.refAddress)
//		{
//			addrStartA = refA.refAddress;
//			addrEndA = refA.address;
//		}
//		else
//		{
//			addrStartA = refA.address;
//			addrEndA = refA.refAddress;
//		}
//
//		for (auto jt = m_referenceList.begin(); jt != m_referenceList.end(); jt++)
//		{
//			auto& refB = jt->get();
//			POINTER addrStartB, addrEndB;
//
//			if (refB.address > refB.refAddress)
//			{
//				addrStartB = refB.refAddress;
//				addrEndB = refB.address;
//			}
//			else
//			{
//				addrStartB = refB.address;
//				addrEndB = refB.refAddress;
//			}
//
//			// refB cover refA
//			bool case1 = addrStartB < addrStartA && addrEndB    > addrEndA;
//
//			// refB inside refA
//			bool case2 = addrStartB >  addrStartA && addrEndB    < addrEndA;
//
//			// refB start inside refA and end outside
//			//bool case3 = addrStartB < addrStartA && addrEndB    > addrStartA && addrEndB < addrEndA;
//
//			// refB start outside refA and end inside
//			//bool case4 = addrStartB >  addrStartA && addrStartB  < addrEndA  && addrEndB >  addrEndA;
//
//			bool case5 = addrStartB == addrStartA;
//			bool case6 = addrEndB == addrEndA;
//			bool case7 = refB.refDepthLevel == refA.refDepthLevel;
//
//			if (case1 && case7) refB.refDepthLevel += 1.f;
//			if (case2 && case7) refB.refDepthLevel += 1.f;
//			//if (case3 && case7) refB.refDepthLevel += 1.f;
//			//if (case4 && case7) refB.refDepthLevel += 1.f;
//
//			//if (case5) refB.refDepthLevel += 1.f;
//			if (case5)
//			{
//				if (addrEndB > addrEndA)
//				{
//					//if (refB.refDepthLevel == 0.f || refB.refDepthLevel == refA.refDepthLevel)
//						refA.refDepthLevel += 1.f;
//					//refB.refDepthLevel += 1.f;
//				}
//				else
//				{
//					//if (refA.refDepthLevel == 0.f || refB.refDepthLevel == refA.refDepthLevel)
//				}
//			}
//			if (case6)
//			{
//				if (addrStartB > addrStartA)
//				{
//					//if (refB.refDepthLevel == 0.f || refB.refDepthLevel == refA.refDepthLevel)
//					refA.refDepthLevel += 1.f;
//					//refB.refDepthLevel += 1.f;
//				}
//				else
//				{
//					//if (refA.refDepthLevel == 0.f || refB.refDepthLevel == refA.refDepthLevel)
//					//refB.refDepthLevel += 1.f;
//				}
//			}
//
//		}
//	}
//}

void ViewDisassembler::Disassemble(Engine::AnalyzerFlags flags)
{
	if (!Engine::IsAttached()) return;

	POINTER startAddress = m_pVirtualBase - 1024;

	try
	{

		Engine::Analyze(startAddress, 2048, flags, m_memoryBuffer, m_analyzedData);

		// find the start index of the address (skip the garbage instructions before it as we -0x10 to the address)
		for (m_instructionOffset = 0; m_instructionOffset < m_analyzedData.instructions.size(); m_instructionOffset++)
		{
			if (m_analyzedData.instructions[m_instructionOffset].address > m_pVirtualBase)
			{
				m_instructionOffset--;
				break;
			}
		}

		if (m_instructionOffset == UPTR_UNDEFINED) m_instructionOffset = 0;

		//// do this to avoid nested hell
		//const SubroutineInfo* pSubroutine = nullptr;

		//for (const SubroutineInfo& subroutine : m_analyzedData.subroutines)
		//{
		//	if (subroutine.start <= m_pVirtualBase && m_pVirtualBase <= subroutine.end)
		//	{
		//		pSubroutine = &subroutine;
		//		break;
		//	}
		//}

		//if (pSubroutine != nullptr)
		//{
		//	auto views = MainView::FindViewsByClass<ViewDecompiler>();
		//	views[0].get().pView->Decompile(pSubroutine->start, pSubroutine->end - pSubroutine->start);
		//}
	}
	catch (Tokio::Exception& e)
	{
		m_instructionOffset = 0;
		m_analyzedData.instructions.clear();
		m_analyzedData.subroutines.clear();

		e.Log("View disassembler failed to analyze\n");
	}
}

void ViewDisassembler::GoToAddress(POINTER address)
{
	m_pVirtualBase = address;
	Disassemble(Engine::AnalyzerFlags_::Symbol |
		Engine::AnalyzerFlags_::CrossReference |
		Engine::AnalyzerFlags_::Subroutine |
		Engine::AnalyzerFlags_::Comment);

	// restore the view of the selected address
	if (auto& selected = m_table.GetSelectedItems(); selected.size() > 0)
	{
		size_t index = selected[0];
		if (index > m_instructionOffset)
			index = m_instructionOffset;

		m_instructionOffset -= index;
		m_table.AddSelectedItem(index);

		m_pVirtualBase = m_analyzedData.instructions[m_instructionOffset].address;
	}
       }