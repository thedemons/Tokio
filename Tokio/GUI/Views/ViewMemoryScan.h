#pragma once
#ifndef TOKIO_GUI_VIEWS_MEMORYSCAN_H
#define TOKIO_GUI_VIEWS_MEMORYSCAN_H

#include "GUI/MainView.h"

class ViewMemoryScan : public BaseView
{
public:
	ViewMemoryScan();

	void Render(bool& bOpen) override;
};

#endif // !TOKIO_GUI_VIEWS_MEMORYSCAN_H