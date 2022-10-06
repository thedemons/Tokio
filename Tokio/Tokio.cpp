#include "stdafx.h"
#include "MainApplication.h"
#include "GUI/MainView.h"
#include "Settings.h"
#include "Common/Exception.h"

void MainLoop()
{
	MainView::Render();
}

int main()
{
	try
	{
		MainApplication::Init();
		Settings::Load();

		MainView::Init();

		MainApplication::SetRenderCallback(MainLoop);
		MainApplication::StartLoop();
		//Settings::Save();

	}
	catch (Tokio::Exception& e)
	{
		e.Log("Critical error");
	}

	return 0;
}