#include "stdafx.h"
#include "ViewSettings.h"
#include "GUI/Widgets/Widgets.hpp"
#include "Engine/Engine.h"
#include "Settings.h"


ViewSettings::ViewSettings()
{
	m_title = ICON_SETTING u8"Settings";
}

void ViewSettings::DrawDisassemblerTab()
{
	auto& settings = Settings::disasm;

	ImGui::BeginChild("##childdisasmSettings");

	ImGui::Checkbox("Show operand size", &settings.bShowOperandSize);
	ImGui::Checkbox("Show operand segments", &settings.bShowOperanSegment);
	ImGui::Checkbox("Show detailed prefix", &settings.bShowDetailedPerfix);
	ImGui::Checkbox("Show hex leading zero", &settings.bShowHexLeadingZero);
	ImGui::Checkbox("Auto padding for hex and decimal", &settings.bAutoHexPadding);

	ImGui::Checkbox("Uppercase prefixes", &settings.bUppercasePrefixes);
	ImGui::Checkbox("Uppercase type cast", &settings.bUppercaseTypeCast);
	ImGui::Checkbox("Uppercase mnemonic", &settings.bUppercaseMnemonic);
	ImGui::Checkbox("Uppercase register", &settings.bUppercaseRegister);
	ImGui::Checkbox("Uppercase decorator", &settings.bUppercaseDecorator);
	ImGui::Checkbox("Uppercase hex", &settings.bUppercaseHex);

	ImGui::SetNextItemWidth(100);
	ImGui::InputText("Hex prefix", settings.cHexPrefix, sizeof(settings.cHexPrefix));

	ImGui::SetNextItemWidth(100);
	ImGui::InputText("Hex suffix", settings.cHexSuffix, sizeof(settings.cHexSuffix));

	ImGui::SetNextItemWidth(100);
	ImGui::InputText("Decimal prefix", settings.cDecPrefix, sizeof(settings.cDecPrefix));

	ImGui::SetNextItemWidth(100);
	ImGui::InputText("Decimal prefix", settings.cDecSuffix, sizeof(settings.cDecSuffix));


	ImGui::EndChild();
	Engine::Disassembler()->UpdateSettings();
}

void ViewSettings::Render(bool& bOpen)
{
	if (m_isOpenned != bOpen)
	{
		m_isOpenned = bOpen;
		if (m_isOpenned)
		{
			ImGui::OpenPopup(Title().c_str());
		}
	}

	if (!m_isOpenned) return;


	//ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 0,0 });
	if (ImGui::BeginPopupModal(Title().c_str(), &m_isOpenned))
	{
		//ImGui::PopStyleVar();
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImVec2 winPadding = ImGui::GetStyle().WindowPadding;
		ImVec2 childSize = windowSize - winPadding * 2.f - ImVec2(0, 50);

		ImGui::BeginChild("##childSettings", childSize, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        if (ImGui::BeginTabBar("##tabSettings", ImGuiTabBarFlags_None))
        {/*
            if (ImGui::BeginTabItem("General"))
            {
                ImGui::EndTabItem();
            }*/
            if (ImGui::BeginTabItem("Disassembler"))
            {
				DrawDisassemblerTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Memory View"))
            {
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

		ImGui::EndChild();

		ImGui::Button("Close");


		ImGui::EndPopup();
	}
	else
	{
		//ImGui::PopStyleVar();
		bOpen = false;
	}
}
