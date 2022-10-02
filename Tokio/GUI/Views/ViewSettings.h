#pragma once
#ifndef TOKIO_GUI_VIEWS_SETTINGS_H
#define TOKIO_GUI_VIEWS_SETTINGS_H

#include "GUI/MainView.h"

class ViewSettings : public BaseView
{
private:
	bool m_isOpenned = false;
	void DrawDisassemblerTab();
public:
	ViewSettings();
	void Render(bool& bOpen) override;
};
#endif // !TOKIO_GUI_VIEWS_SETTINGS_H
