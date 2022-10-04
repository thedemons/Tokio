#pragma once
#ifndef TOKIO_GUI_WIDGETS_HPP
#define TOKIO_GUI_WIDGETS_HPP

// TODO: FIX THE STRUCTURE OF THIS FILE


//#include <string>
//#include <vector>
//
//#include "imgui.hpp"

#include "WidgetsFlags.hpp"
#include "Resources/FontAwesomeImpl.h"

//#include "WTable.h"
//#include "WTreeTable.hpp"
//#include "WTextInput.hpp"
//#include "WPopup.h"


#define ICON_ATTACH_PROC  u8"🖥"
#define ICON_MEMORY_SCAN  u8"🔍"
#define ICON_SYMBOL_LIST  u8"🕹"
#define ICON_DISASSEMBLER u8"♨"
#define ICON_PE_VIEW      u8"👓"
#define ICON_MEMORY_VIEW  u8"💽"
#define ICON_WATCH_LIST   u8"👁"

#define ICON_COPY ICON_FA_CLIPBOARD u8" "
#define ICON_GOTO_ADDRESS ICON_FA_ARROW_RIGHT u8" "
#define ICON_FOLLOW_ADDRESS ICON_FA_ARROW_DOWN u8" "

#define ICON_SUSPEND	  u8"❄"
#define ICON_KILL		  u8"💀"

#define ICON_SETTING      u8"⚙"
#define ICON_WRENCH       u8"🛠"
#define ICON_TOOLBOX      u8"🧰"
#define ICON_SHARE        u8"🔗"
#define ICON_SHARE        u8"🔗"
#define ICON_KEYBOARD     u8"⌨"
#define ICON_KEYBOARD     u8"🖱"
#define ICON_KEYBOARD     u8"💡"
//#define ICON_KEYBOARD   u8"📚"
#define ICON_KEYBOARD     u8"📦"
#define ICON_FILEFOLDER   u8"📦"
#define ICON_FOLDER       u8"📂"
#define ICON_PIN          u8"📌"
#define ICON_RULER        u8"📐"
#define ICON_FIRE         u8"🔥"


namespace Widgets
{

// Get ImGui unique name from a string and a pointer
// The pointer is usually a class that handles the widget
// Or it could be anything... as long as it is unique
_CONSTEXPR20 std::string GetUniqueName(const std::string& name, void* ptr)
{
	return name + "##" + std::to_string(reinterpret_cast<ULONG_PTR>(ptr));
}

template <typename T>
_CONSTEXPR20 ImGuiID GetUniqueID(T uint64_ptr)
{
	return ImHashData(&uint64_ptr, sizeof(T), 0);
}

}

#endif // !TOKIO_GUI_WIDGETS_HPP
