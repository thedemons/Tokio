#include "stdafx.h"
#include "MainApplication.h"
#include "GUI/MainView.h"

void MainLoop()
{
	MainView::Render();
//	ImGui::Begin("Test");
//	ImGui::GetStyle().ItemSpacing.y = 50.f;
//	ImGuiStyle& style = ImGui::GetStyle();
//    ImVec2 windowPosition = ImGui::GetWindowPos();
//    ImVec2 cursorPosition = ImGui::GetCursorPos();
//
//    // this is not a pixel perfect position
//    // you can try to make it more accurate by adding some offset
//    ImVec2 itemPosition(
//        windowPosition.x + cursorPosition.x,
//        windowPosition.y + cursorPosition.y
//    );
//	ImVec2 itemSize = { 100, style.ItemSpacing.y + 13 };
//	auto dl = ImGui::GetForegroundDrawList();
//	auto x = ImGui::GetTextLineHeight();
//    for (int i = 0; i < 20; i++)
//    {
//		auto zxcv = ImGui::GetItemRectMax();
//		itemPosition = windowPosition + ImGui::GetCursorPos();
//        ImGui::Selectable("test123");
//		auto cc = ImGui::GetItemRectSize();
//		dl->AddRect(itemPosition, itemPosition + cc, 0x7f00ff00);
//    }
//
//	ImGui::End();
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

	return 0;
}