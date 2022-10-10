/*
 * Project Tokio
 * Author: thedemons
 * Github: thedemons/Tokio
 * Made:   With love
 *
 * License: MIT License
 *
 * Copyright(c) 2022 thedemons
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#include "stdafx.h"
#include "MainView.h"

#include "Engine/Engine.h"
#include "Widgets/Widgets.hpp"

#include "Views/ViewWatchList.h"
#include "Views/ViewAttachProc.h"
#include "Views/ViewSymbolList.h"
#include "Views/ViewDisassembler.h"
#include "Views/ViewDecompiler.h"
#include "Views/ViewMemoryScan.h"
#include "Views/ViewSettings.h"


namespace MainView
{
void HandlerAttachProcess(std::shared_ptr<ProcessData> target);
void HandlerDetachProcess();

void Init()
{
	AddView<ViewMemoryScan>();
	AddView<ViewWatchList>();
	AddView<ViewAttachProc>();
	AddView<ViewSymbolList>();
	AddView<ViewSettings>();
	AddView<ViewDisassembler>();
	//AddView<ViewDecompiler>();

	Engine::OnAttachCallback(HandlerAttachProcess);
	Engine::OnDetachCallback(HandlerDetachProcess);


	//try
	//{
	//	Engine::Attach(GetCurrentProcessId());
	//}
	//catch (Tokio::Exception& e)
	//{
	//	e.Log("Couldn't attach to the process");
	//}
}

void RenderMenuBar()
{
	bool bOpenAttachProc = false;
	bool bOpenSettings = false;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 7.f, 7.f });
	if (ImGui::BeginMenuBar())
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3.f);

		bOpenAttachProc |= ImGui::Button(u8"🖥");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Attach Process");
			ImGui::EndTooltip();
		}

		bOpenSettings   |= ImGui::Button(ICON_SETTING);

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Open Settings");
			ImGui::EndTooltip();
		}

		if (ImGui::BeginMenuEx("File", u8"123", true))
		{
			bOpenAttachProc |= ImGui::Selectable(u8"🖥 Attach Process");

			if (ImGui::Selectable(u8"💾 Save", false))
			{

			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Views"))
		{
			for (auto& view : g_ViewList)
			{
				if (!view.pView->isClosable()) continue;
				ImGui::MenuItem(view.pView->Title().c_str(), nullptr, &view.bOpen);
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Add new Memory Scan")) AddView<ViewMemoryScan>();
			if (ImGui::MenuItem("Add new Symbol List")) AddView<ViewSymbolList>();
			if (ImGui::MenuItem("Add new Disassembler")) AddView<ViewDisassembler>();

			ImGui::EndMenu();
		}


		ImGui::EndMenuBar();
	}
	ImGui::PopStyleVar();

	// find the ViewAttachProc and open it
	if (bOpenAttachProc)
	{
		SetViewOpen<ViewAttachProc>();
	}

	if (bOpenSettings)
	{
		SetViewOpen<ViewSettings>();
	}
}

void Render()
{
	auto& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;
	colors[ImGuiCol_::ImGuiCol_DockingEmptyBg] = { 0.f, 0.f, 0.f, 0.f };
	static ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	static ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f, 0.f });

	ImGui::Begin("##TokioMainDockSpace", 0, window_flags);

	// must pop those styles before rendering the menu bar
	ImGui::PopStyleVar(3);

	RenderMenuBar();

	// render the dock space
	ImGui::DockSpace(
		ImGui::GetID("##TokioMainDockSpace"),
		{ 0.f, 0.f },
		ImGuiDockNodeFlags_NoWindowMenuButton
	);

	ImGui::End();

	// render view windows
	for (auto& view : g_ViewList)
	{
		view.pView->Render(view.bOpen);
	}
}

void HandlerAttachProcess(std::shared_ptr<ProcessData> target)
{
	for (auto& view : g_ViewList)
	{
		view.pView->OnAttach(target);
	}
}

void HandlerDetachProcess()
{
	for (auto& view : g_ViewList)
	{
		view.pView->OnDetach();
	}
}

}