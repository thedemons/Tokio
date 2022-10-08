#include "stdafx.h"
#include "MainApplication.h"
#include "Engine/Engine.h"
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
		Engine::Init();
		MainView::Init();

		MainApplication::SetRenderCallback(MainLoop);
		MainApplication::StartLoop();
		//Settings::Save();

	}
	catch (Tokio::Exception& e)
	{
		e.Log("Critical error");
	}
	catch (std::exception& e)
	{
		MessageBoxA(0, e.what(), "Error", 16);
	}

	return 0;
}