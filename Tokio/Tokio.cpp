#include "stdafx.h"
#include "MainApplication.h"
#include "GUI/MainView.h"
#include "Settings.h"

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

	Settings::Load();

	MainView::Init();

	MainApplication::SetRenderCallback(MainLoop);
	MainApplication::StartLoop();

	//Settings::Save();
	return 0;
}