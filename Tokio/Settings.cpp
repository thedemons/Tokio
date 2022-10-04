#include "stdafx.h"
#include "Settings.h"


namespace Settings
{

Theme theme;
Disasm disasm;
ShortCuts shortcuts;

void SetDarkVSTheme();

void Load()
{
	SetDarkVSTheme();

	shortcuts.DisasmCopyAddress          = { ImGuiKey_LeftCtrl, ImGuiKey_C, "Copy address"         };
	shortcuts.DisasmGoToAdress           = { ImGuiKey_LeftCtrl, ImGuiKey_G, "Go to address"        };
	shortcuts.DisasmFollowInstruction    = { ImGuiKey_LeftCtrl, ImGuiKey_E, "Follow address"       };
	shortcuts.DisasmGoToReference        = { ImGuiKey_LeftCtrl, ImGuiKey_X, "Go to references"     };
	shortcuts.DisasmAddToWatchList       = { ImGuiKey_LeftCtrl, ImGuiKey_T, "Add to watch list"    };
	shortcuts.DisasmOpenInMemoryView     = { ImGuiKey_LeftCtrl, ImGuiKey_B, "Open in memory view"  };
	shortcuts.DisasmSwitchMode           = { ImGuiKey_Space               , "Switch mode"          };

	shortcuts.SymListCopyAddress         = { ImGuiKey_LeftCtrl, ImGuiKey_C, "Copy address"         };
	shortcuts.SymListRefresh             = { ImGuiKey_LeftCtrl, ImGuiKey_R, "Refresh symbols"      };
	shortcuts.SymListAddToWatchList      = { ImGuiKey_LeftCtrl, ImGuiKey_T, "Add to watch list"    };
	shortcuts.SymListOpenInMemoryView    = { ImGuiKey_LeftCtrl, ImGuiKey_B, "Open in memory view"  };
	shortcuts.SymListOpenInDisassembler  = { ImGuiKey_LeftCtrl, ImGuiKey_D, "Open in disassembler" };

	shortcuts.AttachProcAttach           = { ImGuiKey_A       ,             "Attach process"       };
	shortcuts.AttachProcOpenFileLocation = { ImGuiKey_LeftCtrl, ImGuiKey_E, "Open file location"   };
	shortcuts.AttachProcSuspendProcess   = { ImGuiKey_LeftCtrl, ImGuiKey_Q, "Suspend process"      };
	shortcuts.AttachProcKillProcess      = { ImGuiKey_LeftCtrl, ImGuiKey_W, "Kill process"         };
}

void Theme::Serialize(SerializeStream & stream) const 
{
	stream << type << customThemePath << disasm;
}

void Theme::Deserialize(SerializeStream& stream)
{
	stream >> type >> customThemePath >> disasm;
}

void SetDarkVSTheme()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.ChildRounding = 0.f;
	style.FrameRounding = 0.f;
	style.GrabRounding = 0.f;
	style.PopupRounding = 0.f;
	style.ScrollbarRounding = 0.f;
	style.TabRounding = 0.f;
	style.WindowRounding = 0.f;

	style.FrameBorderSize = 1.f;

	ImVec4* colors = style.Colors;

	colors[ImGuiCol_WindowBg]              = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_ChildBg]               = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_PopupBg]               = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

	colors[ImGuiCol_Border]                = ImVec4(0.22f, 0.22f, 0.22f, 0.80f);
	colors[ImGuiCol_BorderHovered]         = ImVec4(0.62f, 0.62f, 0.62f, 0.80f);
	colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
	colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
	colors[ImGuiCol_FrameBgActive]         = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
	colors[ImGuiCol_TitleBg]               = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
	colors[ImGuiCol_TitleBgActive]         = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
	colors[ImGuiCol_MenuBarBg]             = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.24f, 0.24f, 0.26f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.62f, 0.62f, 0.62f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.94f, 0.92f, 0.94f, 1.00f);
	colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button]                = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
	colors[ImGuiCol_ButtonHovered]         = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
	colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);

	colors[ImGuiCol_Header]                = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_HeaderHovered]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_HeaderActive]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);

	colors[ImGuiCol_Separator]             = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive]       = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip]            = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.94f, 0.92f, 0.94f, 1.00f);
	colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.94f, 0.92f, 0.94f, 1.00f);
	colors[ImGuiCol_Tab]                   = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
	colors[ImGuiCol_TabHovered]            = ImVec4(0.11f, 0.59f, 0.92f, 1.00f);
	colors[ImGuiCol_TabActive]             = ImVec4(0.00f, 0.48f, 0.80f, 1.00f);
	colors[ImGuiCol_TabUnfocused]          = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.00f, 0.48f, 0.80f, 1.00f);
	colors[ImGuiCol_DockingPreview]        = ImVec4(0.58f, 0.51f, 1.00f, 0.70f);
	colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.00f, 1.00f, 0.02f, 1.00f);
	colors[ImGuiCol_PlotHistogram]         = ImVec4(0.14f, 1.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.14f, 1.00f, 0.00f, 1.00f);


	// Table
	colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.24f, 0.24f, 0.25f, 1.00f);

	colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.25f, 0.25f, 0.27f, 0.70f);
	colors[ImGuiCol_TableBorderLight]      = ImVec4(0.26f, 0.26f, 0.27f, 0.70f);

	colors[ImGuiCol_TableRowBg]            = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

	colors[ImGuiCol_TableRowHovered]       = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TableRowActive]        = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowClicked]       = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);

	// 
	colors[ImGuiCol_Text]                  = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
	colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

	colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.00f, 0.00f, 0.00f, 0.40f);


	colors[ImGuiCol_TextAddress]           = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextAddressModule]     = ImVec4(0.31f, 0.79f, 0.69f, 1.00f);
	colors[ImGuiCol_TextFunction]          = ImVec4(0.86f, 0.86f, 0.67f, 1.00f);
	colors[ImGuiCol_TextModule]            = ImVec4(0.34f, 0.61f, 0.84f, 1.00f);

	colors[ImGuiCol_TextBinary]            = ImVec4(0.72f, 0.84f, 0.64f, 1.00f);
	colors[ImGuiCol_TextDecimal]           = ImVec4(0.72f, 0.84f, 0.64f, 1.00f);
	colors[ImGuiCol_TextHex]               = ImVec4(0.72f, 0.84f, 0.64f, 1.00f);


	// ABGR format
	theme.disasm.Invalid                   = 0xFF797979;
	theme.disasm.Delimeter                 = 0xFFB4B4B4;
	theme.disasm.ParenthesisOpen           = 0xFFB4B4B4;
	theme.disasm.ParenthesisClose          = 0xFFB4B4B4;
	theme.disasm.Prefix                    = 0xFFB4C9E8;
	theme.disasm.Mnemonic                  = 0xFFF4F4F4;
	theme.disasm.Register                  = 0xFFFEDC9C;
	theme.disasm.AddressAbs                = 0xFF4AA657;
	theme.disasm.AddressRel                = 0xFFB4B4B4;
	theme.disasm.Displacement              = 0xFFA8CEB5;
	theme.disasm.Immediate                 = 0xFFA8CEB5;
	theme.disasm.TypeCast                  = 0xFFC086C5;
	theme.disasm.Decorator                 = 0xFF859DD6;
	theme.disasm.Literal                   = 0xFF859DD6;

	theme.disasm.Address                   = 0xFFf2f2f2;
	theme.disasm.Bytes                     = 0xFFcfcfcf;
	theme.disasm.Function                  = 0xFFAADCDC;
	theme.disasm.Module                    = 0xFFB0C94E;
	theme.disasm.String                    = 0xFF859DD6;
	theme.disasm.Xref                      = 0xFFB9EBEB;
	theme.disasm.ReferenceHighLight        = 0xFFDFA0D8;

	theme.disasm.mneCall                   = 0xFF3EC431;
	theme.disasm.mneSyscall                = 0xFFFFB7BE;
	theme.disasm.mneJump                   = 0xFF55B155;
	theme.disasm.mneJumpCondition          = 0xFF32B1D0;
	theme.disasm.mneReturn                 = 0xFF859DD6;
	theme.disasm.mneInt3				   = 0xFF787878;

}

}