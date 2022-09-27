#pragma once
#include "stdafx.h"
#include "MainView.h"
#include "Themes.hpp"

#include "ViewScanner.hpp"
#include "ViewWatchList.hpp"
#include "ViewAttachProc.hpp"

struct ViewListData
{
	BaseView* pView = nullptr;
	bool bOpen = false;
};

std::vector<ViewListData> m_ViewList;

namespace MainView
{

void Init()
{


	ThemeSettings::SetDarkVSTheme();

	m_ViewList.push_back(
		{ static_cast<BaseView*>(new ViewScanner()), true }
	);
	m_ViewList.push_back(
		{ static_cast<BaseView*>(new ViewWatchList()), true }
	);
	m_ViewList.push_back(
		{ static_cast<BaseView*>(new ViewAttachProc()), true }
	);
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

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("##TokioDockSpace", 0, window_flags);

	ImGui::PopStyleVar(3);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::Selectable("Attach Process");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Views"))
		{
			for (auto& view : m_ViewList)
			{
				if (!view.pView->isClosable()) continue;
				ImGui::MenuItem(view.pView->Title().c_str(), nullptr, &view.bOpen);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}


	ImGuiID dockspace_id = ImGui::GetID("##TokioDockSpace");
	ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlagsPrivate_::ImGuiDockNodeFlags_NoWindowMenuButton;

	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockSpaceFlags);

	ImGui::End();

	for (auto& view : m_ViewList)
	{
		view.pView->Render(&view.bOpen);
	}
}
}