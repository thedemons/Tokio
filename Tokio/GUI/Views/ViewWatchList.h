#pragma once
#ifndef TOKIO_GUI_VIEWS_WATCH_LIST_H
#define TOKIO_GUI_VIEWS_WATCH_LIST_H

#include "GUI/MainView.h"

class ViewWatchList : public BaseView
{
public:
	ViewWatchList();
	void Render(bool& bOpen) override;
};
#endif // !TOKIO_GUI_VIEWS_WATCH_LIST_H
