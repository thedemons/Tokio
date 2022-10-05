#include "stdafx.h"
#include "ShortcutKey.h"
#include "Resources/FontAwesomeImpl.h"

ShortcutKey::ShortcutKey()
{

}

ShortcutKey::ShortcutKey(const ImGuiKey hold, const ImGuiKey press, const char* description, const ImGui::TokenizedText* icon) :
	m_holdKey(hold), m_pressKey(press)
{
	if (description) m_description = description;
	if (icon) m_icon = *icon;

	UpdateName();
}

ShortcutKey::ShortcutKey(const ImGuiKey press, const char* description, const ImGui::TokenizedText* icon) :
	m_pressKey(press)
{
	if (description) m_description = description;
	if (icon) m_icon = *icon;

	UpdateName();
}

static bool IsRootOfOpenMenuSet()
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	if ((g.OpenPopupStack.Size <= g.BeginPopupStack.Size) || (window->Flags & ImGuiWindowFlags_ChildMenu))
		return false;

	// Initially we used 'upper_popup->OpenParentId == window->IDStack.back()' to differentiate multiple menu sets from each others
	// (e.g. inside menu bar vs loose menu items) based on parent ID.
	// This would however prevent the use of e.g. PuhsID() user code submitting menus.
	// Previously this worked between popup and a first child menu because the first child menu always had the _ChildWindow flag,
	// making  hovering on parent popup possible while first child menu was focused - but this was generally a bug with other side effects.
	// Instead we don't treat Popup specifically (in order to consistently support menu features in them), maybe the first child menu of a Popup
	// doesn't have the _ChildWindow flag, and we rely on this IsRootOfOpenMenuSet() check to allow hovering between root window/popup and first child menu.
	// In the end, lack of ID check made it so we could no longer differentiate between separate menu sets. To compensate for that, we at least check parent window nav layer.
	// This fixes the most common case of menu opening on hover when moving between window content and menu bar. Multiple different menu sets in same nav layer would still
	// open on hover, but that should be a lesser problem, because if such menus are close in proximity in window content then it won't feel weird and if they are far apart
	// it likely won't be a problem anyone runs into.
	const ImGuiPopupData* upper_popup = &g.OpenPopupStack[g.BeginPopupStack.Size];
	return (window->DC.NavLayerCurrent == upper_popup->ParentNavLayer && upper_popup->Window && (upper_popup->Window->Flags & ImGuiWindowFlags_ChildMenu));
}


bool ShortcutKey::RenderInPopup(bool bEnabled)
{
	//if (ImGui::MenuItem(m_description.c_str(), m_name.c_str(), false, bEnabled))
	//	m_overridePress = true;


	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	ImGuiStyle& style = g.Style;
	ImVec2 pos = window->DC.CursorPos;
	ImVec2 icon_size = m_icon.CalcSize();
	ImVec2 label_size = icon_size + ImGui::CalcTextSize(m_description.c_str(), m_description.c_str() + m_description.size(), false);

	const bool menuset_is_open = IsRootOfOpenMenuSet();
	ImGuiWindow* backed_nav_window = g.NavWindow;
	if (menuset_is_open)
		g.NavWindow = window;

	// We've been using the equivalent of ImGuiSelectableFlags_SetNavIdOnHover on all Selectable() since early Nav system days (commit 43ee5d73),
	// but I am unsure whether this should be kept at all. For now moved it to be an opt-in feature used by menus only.
	bool pressed;
	ImGui::PushID(this);
	if (!bEnabled)
		ImGui::BeginDisabled();

	const ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SelectOnRelease | ImGuiSelectableFlags_SetNavIdOnHover;
	const ImGuiMenuColumns* offsets = &window->DC.MenuColumns;
	if (window->DC.LayoutType == ImGuiLayoutType_Horizontal)
	{
		// Mimic the exact layout spacing of BeginMenu() to allow MenuItem() inside a menu bar, which is a little misleading but may be useful
		// Note that in this situation: we don't render the shortcut, we render a highlight instead of the selected tick mark.
		float w = label_size.x;
		window->DC.CursorPos.x += IM_FLOOR(style.ItemSpacing.x * 0.5f);
		ImVec2 text_pos(window->DC.CursorPos.x + offsets->OffsetLabel, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x * 2.0f, style.ItemSpacing.y));
		pressed = ImGui::Selectable("", false, selectable_flags, ImVec2(w, 0.0f));
		ImGui::PopStyleVar();

		m_icon.Render(g.CurrentWindow->DrawList, text_pos);
		ImGui::RenderText(text_pos + ImVec2(icon_size.x, 0), m_description.c_str(), m_description.c_str() + m_description.size());

		window->DC.CursorPos.x += IM_FLOOR(style.ItemSpacing.x * (-1.0f + 0.5f)); // -1 spacing to compensate the spacing added when Selectable() did a SameLine(). It would also work to call SameLine() ourselves after the PopStyleVar().
	}
	else
	{
		float icon_w = 0.0f;
		float shortcut_w = ImGui::CalcTextSize(m_name.c_str(), m_name.c_str() + m_name.size()).x;
		float checkmark_w = IM_FLOOR(g.FontSize * 1.20f);
		float min_w = window->DC.MenuColumns.DeclColumns(icon_w, label_size.x, shortcut_w, checkmark_w); // Feedback for next frame
		float stretch_w = ImMax(0.0f, ImGui::GetContentRegionAvail().x - min_w);
		pressed = ImGui::Selectable("", false, selectable_flags | ImGuiSelectableFlags_SpanAvailWidth, ImVec2(min_w, 0.0f));

		ImVec2 text_pos = pos + ImVec2(offsets->OffsetLabel, 0.0f);
		m_icon.Render(g.CurrentWindow->DrawList, text_pos);
		ImGui::RenderText(text_pos + ImVec2(icon_size.x ? 20.f : 0.f, 0), m_description.c_str(), m_description.c_str() + m_description.size());

		if (shortcut_w > 0.0f)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]);
			ImGui::RenderText(pos + ImVec2(offsets->OffsetShortcut + stretch_w, 0.0f), m_name.c_str(), m_name.c_str() + m_name.size(), false);
			ImGui::PopStyleColor();
		}
	}
	IMGUI_TEST_ENGINE_ITEM_INFO(g.LastItemData.ID, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (selected ? ImGuiItemStatusFlags_Checked : 0));
	if (!bEnabled) ImGui::EndDisabled();

	ImGui::PopID(); 

	if (menuset_is_open) g.NavWindow = backed_nav_window;
	if (pressed) m_overridePress = true;

	return pressed;
}

void ShortcutKey::RenderEdittor()
{

}

void ShortcutKey::UpdateName()
{
	static const std::map<ImGuiKey, std::string> mapName = {
		{ ImGuiKey_None          , "None"                  },
		{ ImGuiKey_Tab           , "Tab"                   },
		{ ImGuiKey_LeftArrow     , ICON_FA_ARROW_LEFT      },
		{ ImGuiKey_RightArrow    , ICON_FA_ARROW_RIGHT     },
		{ ImGuiKey_UpArrow       , ICON_FA_ARROW_UP        },
		{ ImGuiKey_DownArrow     , ICON_FA_ARROW_DOWN      },
		{ ImGuiKey_PageUp        , "PageUp"                },
		{ ImGuiKey_PageDown      , "PageDown"              },
		{ ImGuiKey_Home          , "Home"                  },
		{ ImGuiKey_End           , "End"                   },
		{ ImGuiKey_Insert        , "Insert"                },
		{ ImGuiKey_Delete        , "Del"                   },
		{ ImGuiKey_Backspace     , ICON_FA_LONG_ARROW_LEFT },
		{ ImGuiKey_Space         , "Space"                 },
		{ ImGuiKey_Enter         , "Enter"                 },
		{ ImGuiKey_Escape        , "Escape"                },
		{ ImGuiKey_LeftCtrl      , "LCtrl"                 },
		{ ImGuiKey_LeftShift     , "LShift"                },
		{ ImGuiKey_LeftAlt       , "LAlt"                  },
		{ ImGuiKey_LeftSuper     , "LSuper"                },
		{ ImGuiKey_RightCtrl     , "RCtrl"                 },
		{ ImGuiKey_RightShift    , "RShift"                },
		{ ImGuiKey_RightAlt      , "RAlt"                  },
		{ ImGuiKey_RightSuper    , "RSuper"                },
		{ ImGuiKey_Menu          , "Menu"                  },
		{ ImGuiKey_0             , "0"                     },
		{ ImGuiKey_1             , "1"                     },
		{ ImGuiKey_2             , "2"                     },
		{ ImGuiKey_3             , "3"                     },
		{ ImGuiKey_4             , "4"                     },
		{ ImGuiKey_5             , "5"                     },
		{ ImGuiKey_6             , "6"                     },
		{ ImGuiKey_7             , "7"                     },
		{ ImGuiKey_8             , "8"                     },
		{ ImGuiKey_9             , "9"                     },
		{ ImGuiKey_A             , "A"                     },
		{ ImGuiKey_B             , "B"                     },
		{ ImGuiKey_C             , "C"                     },
		{ ImGuiKey_D             , "D"                     },
		{ ImGuiKey_E             , "E"                     },
		{ ImGuiKey_F             , "F"                     }, 
		{ ImGuiKey_G             , "G"                     }, 
		{ ImGuiKey_H             , "H"                     },
		{ ImGuiKey_I             , "I"                     },
		{ ImGuiKey_J             , "J"                     },
		{ ImGuiKey_K             , "K"                     }, 
		{ ImGuiKey_L             , "L"                     }, 
		{ ImGuiKey_M             , "M"                     }, 
		{ ImGuiKey_N             , "N"                     },
		{ ImGuiKey_O             , "O"                     }, 
		{ ImGuiKey_P             , "P"                     }, 
		{ ImGuiKey_Q             , "Q"                     }, 
		{ ImGuiKey_R             , "R"                     }, 
		{ ImGuiKey_S             , "S"                     },
		{ ImGuiKey_T             , "T"                     },
		{ ImGuiKey_U             , "U"                     }, 
		{ ImGuiKey_V             , "V"                     }, 
		{ ImGuiKey_W             , "W"                     }, 
		{ ImGuiKey_X             , "X"                     }, 
		{ ImGuiKey_Y             , "Y"                     }, 
		{ ImGuiKey_Z             , "Z"                     },
		{ ImGuiKey_F1            , "F1"                    },
		{ ImGuiKey_F2            , "F2"                    }, 
		{ ImGuiKey_F3            , "F3"                    }, 
		{ ImGuiKey_F4            , "F4"                    }, 
		{ ImGuiKey_F5            , "F5"                    },
		{ ImGuiKey_F6            , "F6"                    },
		{ ImGuiKey_F7            , "F7"                    },
		{ ImGuiKey_F8            , "F8"                    },
		{ ImGuiKey_F9            , "F9"                    },
		{ ImGuiKey_F10           , "F10"                   }, 
		{ ImGuiKey_F11           , "F11"                   },
		{ ImGuiKey_F12           , "F12"                   },
		{ ImGuiKey_Apostrophe    , "Apostrophe"            },
		{ ImGuiKey_Comma         , ","                     },
		{ ImGuiKey_Minus         , "-"                     },
		{ ImGuiKey_Period        , "."                     },
		{ ImGuiKey_Slash         , "."                     }, 
		{ ImGuiKey_Semicolon     , ";"                     }, 
		{ ImGuiKey_Equal         , "="                     },
		{ ImGuiKey_LeftBracket   , "{"                     },
		{ ImGuiKey_Backslash     , "\\"                    },
		{ ImGuiKey_RightBracket  , "}"                     },
		{ ImGuiKey_GraveAccent   , "`"                     },
		{ ImGuiKey_CapsLock      , "Caps"                  },
		{ ImGuiKey_ScrollLock    , "ScrLk"                 },
		{ ImGuiKey_NumLock       , "NumLock"               },
		{ ImGuiKey_PrintScreen   , "PrtSrc"                },
		{ ImGuiKey_Pause         , "Pause"                 },
		{ ImGuiKey_Keypad0       , "Num0"                  },
		{ ImGuiKey_Keypad1       , "Num1"                  },
		{ ImGuiKey_Keypad2       , "Num2"                  },
		{ ImGuiKey_Keypad3       , "Num3"                  },
		{ ImGuiKey_Keypad4       , "Num4"                  },
		{ ImGuiKey_Keypad5       , "Num5"                  },
		{ ImGuiKey_Keypad6       , "Num6"                  },
		{ ImGuiKey_Keypad7       , "Num7"                  },
		{ ImGuiKey_Keypad8       , "Num8"                  }, 
		{ ImGuiKey_Keypad9       , "Num9"                  },
		{ ImGuiKey_KeypadDecimal , "NumDecimal"            },
		{ ImGuiKey_KeypadDivide  , "Num/"                  },
		{ ImGuiKey_KeypadMultiply, "Num*"                  },
		{ ImGuiKey_KeypadSubtract, "Num-"                  },
		{ ImGuiKey_KeypadAdd     , "Num+"                  },
		{ ImGuiKey_KeypadEnter   , "NumEnter"              },
		{ ImGuiKey_KeypadEqual   , "Num="                  },
	};

	if (m_holdKey != ImGuiKey_None && m_pressKey != ImGuiKey_None)
	{
		m_name = mapName.find(m_holdKey)->second + " + " + mapName.find(m_pressKey)->second;
	}
	else if (m_pressKey != ImGuiKey_None)
	{
		m_name = mapName.find(m_pressKey)->second;
	}
	else
	{
		m_name = "None";
	}
}