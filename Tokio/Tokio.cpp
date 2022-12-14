/*
 * Project Tokio
 * Author: thedemons
 * Github: thedemons/Tokio
 * Made:   With love
 *
 * License: MIT License
 *
 * Copyright(c) 2022 thedemons
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "stdafx.h"
#include "Graphics.h"
#include "Engine/Engine.h"
#include "GUI/MainView.h"
#include "Settings.h"
#include "Common/Exception.h"

void MainLoop()
{

	while (Graphics::BeginRender())
	{
		ImGui::NewFrame();

		MainView::Render();
		static char fps_info[1024];
		sprintf_s(fps_info, "FPS: %.2f", ImGui::GetIO().Framerate);

		auto* viewport = ImGui::GetMainViewport();
		auto* drawlist = ImGui::GetForegroundDrawList();
		drawlist->AddText(viewport->Pos + ImVec2(viewport->Size.x - 100.f, 5.f), 0xffffffff, fps_info);


		ImGui::Render();
		Graphics::EndRender();
	}

	Graphics::CleanUp();
}

void InitGraphics()
{
	Graphics::InitWindow();
	Graphics::InitDevice();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	
	io.IniFilename = "Tokio.ini";

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.ConfigDockingAlwaysTabBar = false;
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigDockingTransparentPayload = true;

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowMinSize = { 300.f, 50.f };

	ImGui::StyleColorsDark();

	Graphics::InitImgui();
	Graphics::LoadFont();
}

int main()
{
	try
	{
		Tokio::Log("Initilizing graphics\n");
		InitGraphics();

		Tokio::Log("Loading Settings\n");
		Settings::Load();

		Tokio::Log("Initilizing Engine\n");
		Engine::Init();

		Tokio::Log("Settings up Views\n");
		MainView::Init();

		Tokio::Log("Tokio started!\n");
		Graphics::ShowWindow();
		MainLoop();
	}
	catch (Tokio::Exception& e)
	{
		e.Log("Critical error");
	}

	return 0;
}