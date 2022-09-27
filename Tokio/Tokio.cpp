#include "stdafx.h"
#include "MainView.h"

void MainLoop()
{
	MainView::Render();
}

int main()
{
	auto initResult = MainWindow::Init();
	if (initResult.has_error())
	{
		initResult.error().show(L"Cannot initialize window");
		return 1;
	}


	MainView::Init();
	MainWindow::SetRenderCallback(MainLoop);
	MainWindow::StartLoop();

	return 0;
}