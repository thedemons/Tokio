#pragma once
#ifndef TOKIO_MAIN_VIEW_H
#define TOKIO_MAIN_VIEW_H
#include "../MainApplication.h"
#include "Engine/EngineDef.h"
#include "Views/BaseView.hpp"

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

inline std::vector<ViewWindowData> m_ViewList;

void Init();
void Render();

template <typename ViewType>
auto FindViewByClass() -> SafeResult(TemplateWindowData<ViewType>&)
{
	for (auto& view : m_ViewList)
		if (dynamic_cast<ViewType*>(view.pView)) return reinterpret_cast<TemplateWindowData<ViewType>&>(view);

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

#endif // !TOKIO_MAIN_VIEW_H