#pragma once
#include "stdafx.h"
#include "MainView.h"

#include "Engine/Engine.h"
#include "Widgets/Widgets.hpp"

#include "Views/ViewWatchList.h"
#include "Views/ViewAttachProc.h"
#include "Views/ViewSymbolList.h"
#include "Views/ViewDisassembler.h"
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

	Engine::OnAttachCallback(HandlerAttachProcess);
	Engine::OnDetachCallback(HandlerDetachProcess);


	try
	{
		Engine::Attach(177508);
	}
	catch (Tokio::Exception& e)
	{
		e.Log("Couldn't attach to the process");
	}
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