#pragma once
#ifndef TOKIO_MAIN_VIEW_H
#define TOKIO_MAIN_VIEW_H
#include "../MainApplication.h"
#include "Views/BaseView.hpp"

namespace MainView
{
struct ViewWindowData
{
	BaseView* pView = nullptr;
	bool bOpen = false;
};

inline std::vector<ViewWindowData> m_ViewList;

void Init();
void Render();

template <typename ViewType>
auto FindViewByClass() -> SafeResult(ViewWindowData&)
{
	for (auto& view : m_ViewList)
		if (dynamic_cast<ViewType*>(view.pView)) return view;

	return cpp::fail(common::err(common::errcode::CannotFindTheViewWindow));
};

template <typename ViewType>
auto FindMultipleViewByClass() -> std::vector<ViewType*>
{
	std::vector<ViewType*> result;

	for (auto& view : m_ViewList)
		if (dynamic_cast<ViewType*>(view.pView)) result.push_back(static_cast<ViewType*>(view.pView));

	return result;
};

}

namespace GUIUtils
{
// Get ImGui unique name from a string and a pointer
// The pointer is usually a class that handles the widget
// Or it could be anything... as long as it is unique
_CONSTEXPR20 std::string GetUniqueName(const std::string& name, void* ptr)
{
	return name + "##" + std::to_string(reinterpret_cast<ULONG_PTR>(ptr));
}

template <typename T>
_CONSTEXPR20 ImGuiID GetUniqueID(T uint64_ptr)
{
	return ImHashData(&uint64_ptr, sizeof(T), 0);
}

}
#endif // TOKIO_MAIN_VIEW_H