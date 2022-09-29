#pragma once
#include "stdafx.h"
#include "MainView.h"
#include "Themes.hpp"

#include "Resources/FontAwesomeImpl.h"
#include "Widgets/WTable.hpp"
#include "Widgets/WTextInput.hpp"
#include "Widgets/WPopup.hpp"
#include "Views/ViewScanner.hpp"
//#include "Views/ViewTreeTable.hpp"
#include "Views/ViewWatchList.hpp"
#include "Views/ViewAttachProc.hpp"
#include "Views/ViewModules.hpp"


namespace MainView
{

void Init()
{
	ThemeSettings::SetDarkVSTheme();

	BaseView* viewScanner = new ViewScanner();
	BaseView* viewWatchList = new ViewWatchList();
	BaseView* viewAttachProc = new ViewAttachProc();
	BaseView* viewModules = new ViewModules();

	m_ViewList.push_back({ viewScanner, viewScanner->isDefaultOpen() });
	m_ViewList.push_back({ viewWatchList, viewWatchList->isDefaultOpen() });
	m_ViewList.push_back({ viewAttachProc, viewAttachProc->isDefaultOpen() });
	m_ViewList.push_back({ viewModules, viewModules->isDefaultOpen() });
}

void RenderMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::Selectable(u8"🖥 Attach Process"))
			{
				auto viewAttachProc = FindViewByClass<ViewAttachProc>();

				if (viewAttachProc.has_error()) viewAttachProc.error().show();
				else viewAttachProc.value().bOpen = true;
			}
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
	for (auto& view : m_ViewList)
	{
		view.pView->Render(view.bOpen);
	}
}



}