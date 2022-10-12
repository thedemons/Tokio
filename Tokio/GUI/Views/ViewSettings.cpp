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
#include "ViewSettings.h"
#include "GUI/Widgets/Widgets.hpp"
#include "Engine/Engine.h"
#include "Settings.h"


ViewSettings::ViewSettings()
{
	m_title = ICON_TOKIO_SETTINGS " Settings";
}

void DrawGeneralTab()
{
	auto& settings = Settings::general;

	ImGui::BeginChild("##childdgeneralSettings");

	ImGui::Checkbox("Enable v-sync", &settings.bEnableVsync);


	ImGui::EndChild();
	Engine::Disassembler()->UpdateSettings();
}

void DrawDisassemblerTab()
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
        {
            if (ImGui::BeginTabItem("General"))
            {
				DrawGeneralTab();
                ImGui::EndTabItem();
            }

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
