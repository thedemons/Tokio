#include "stdafx.h"
#include "ShortcutKey.h"
#include "Resources/FontAwesomeImpl.h"

ShortcutKey::ShortcutKey()
{

}

ShortcutKey::ShortcutKey(const ImGuiKey press, const char* description) : 
	m_pressKey(press)
{
	m_description = description;
	UpdateName();
}

ShortcutKey::ShortcutKey(const ImGuiKey hold, const ImGuiKey press, const char* description) : 
	m_holdKey(hold), m_pressKey(press)
{
	m_description = description;
	UpdateName();
}

void ShortcutKey::RenderInPopup(bool bEnabled)
{
	if (ImGui::MenuItem(m_description.c_str(), m_name.c_str(), false, bEnabled))
		m_overridePress = true;
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