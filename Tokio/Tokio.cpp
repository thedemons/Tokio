#include "stdafx.h"
#include "MainApplication.h"
#include "GUI/MainView.h"

// ikr
void MainLoop()
{
	MainView::Render();
}

int main()
{
	auto initResult = MainApplication::Init();
	if (initResult.has_error())
	{
		initResult.error().show(L"Cannot initialize window");
		return 1;
	}

	Settings::data.Load();

	MainView::Init();

	MainApplication::SetRenderCallback(MainLoop);
	MainApplication::StartLoop();

	Settings::data.Save();
	return 0;
}