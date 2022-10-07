#pragma once
#ifndef TOKIO_MAIN_VIEW_H
#define TOKIO_MAIN_VIEW_H

#include "MainApplication.h"
#include "Views/BaseView.hpp"
#include "Common/Exception.h"

namespace MainView
{
struct ViewWindowData
{
	BaseView* pView = nullptr;
	bool bOpen = false;
};

template <typename ViewType>
struct TemplateWindowData
{
	ViewType* pView = nullptr;
	bool bOpen = false;
};

inline std::vector<ViewWindowData> g_ViewList;

void Init();
void Render();

// gee look at this nested template hell...
template <typename ViewType>
std::vector<std::reference_wrapper<TemplateWindowData<ViewType>>> 
FindViewsByClass() noexcept
{
	std::vector<std::reference_wrapper<TemplateWindowData<ViewType>>> result;

	for (auto& view : g_ViewList)
	{
		if (dynamic_cast<ViewType*>(view.pView))
		{
			result.push_back(reinterpret_cast<TemplateWindowData<ViewType>&>(view));
		}
	}
	return result;
};

// get the numbered view name
// for instance: "Disassembler 3" if there are already two other windows opened
template <typename ViewType>
std::string GetViewTitle(const std::string& title) noexcept
{
	auto otherViews = FindViewsByClass<ViewType>();
	if (otherViews.size() == 0) return title;

	std::string copy = title;
	copy += " " + std::to_string(otherViews.size() + 1);
	return copy;
}

// open a specific view, pass UPTR_UNDEFINED to open all views found
template <typename ViewType>
void SetViewOpen(size_t index = 0, bool bOpen = true) noexcept
{
	auto views = FindViewsByClass<ViewType>();
	if (views.size() == 0)
	{
		Tokio::Log("Failed to find any view?");
	}
	else
	{
		if (index == UPTR_UNDEFINED)
		{
			for (auto& view : views)
			{
				view.get().bOpen = bOpen;
			}
		}
		else if (index < views.size())
		{
			views[index].get().bOpen = bOpen;
		}
		else
		{
			Tokio::Log("The view index to open was wrong");
		}
	}
}

// focus a specific view
template <typename ViewType>
void SetViewFocus(size_t index = 0) noexcept
{
	auto views = FindViewsByClass<ViewType>();
	if (views.size() == 0)
	{
		Tokio::Log("Failed to find any view?");
	}
	else
	{
		if (index < views.size())
		{

			auto& view = views[index].get();
			view.bOpen = true;

			ImGuiWindow* window = ImGui::FindWindowByName(view.pView->Title().c_str());
			ImGui::FocusWindow(window);
		}
		else
		{
			Tokio::Log("The view index to open was wrong");
		}
	}
}

template <typename ViewType>
TemplateWindowData<ViewType>& AddView() noexcept
{
	auto& view = g_ViewList.emplace_back(ViewWindowData{ new ViewType(), false });
	view.bOpen = view.pView->isDefaultOpen();

	return reinterpret_cast<TemplateWindowData<ViewType>&>(view);
}

}


#endif // !TOKIO_MAIN_VIEW_H