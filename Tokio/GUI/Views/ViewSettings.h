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


	_CONSTEXPR20 bool isClosable() const  override
	{
		return false;
	}

	_CONSTEXPR20 bool isDefaultOpen()  const override
	{
		return false;
	}

	_CONSTEXPR20 bool isAllowMultipleViews()  const override
	{
		return false;
	}
};
#endif // !TOKIO_GUI_VIEWS_SETTINGS_H
