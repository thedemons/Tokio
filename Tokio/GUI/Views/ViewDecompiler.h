#pragma once
#ifndef TOKIO_GUI_VIEWS_DECOMPILER_H
#define TOKIO_GUI_VIEWS_DECOMPILER_H

#include "GUI/MainView.h"
#include "GUI/Widgets/Widgets.hpp"
#include "GUI/Widgets/WTextEditor.h"

class ViewDecompiler : public BaseView
{
private: 
	Widgets::TextEditor m_textEditor;
public:
	ViewDecompiler();

	void Render(bool& bOpen) override;

	void Decompile(POINTER address, size_t size);
};

#endif // !TOKIO_GUI_VIEWS_MEMORYSCAN_H