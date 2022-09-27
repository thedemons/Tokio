#pragma once
#include "MainWindow.h"
#include "BaseView.hpp"

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

}