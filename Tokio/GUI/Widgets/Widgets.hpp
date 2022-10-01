#pragma once

namespace Widgets
{
#define WIDGET_ENUM_OPERATORS(EnumType) \
DEFINE_ENUM_FLAG_OPERATORS(TableFlags) \
inline bool operator!(TableFlags a) { return !static_cast<bool>(a); } \
inline bool operator==(TableFlags a, UINT b) { return static_cast<UINT>(a) == b; }

enum class TableFlags : UINT
{
	None		= 0,
	NoHeader	= 1 << 0,

}; WIDGET_ENUM_OPERATORS(TableFlags)

}

#define ICON_ATTACH_PROC  u8"🖥"
#define ICON_MEMORY_SCAN  u8"🔍"
#define ICON_SYMBOL_LIST  u8"🕹"
#define ICON_DISASSEMBLER u8"🖥"
#define ICON_PE_VIEW      u8"👓"
#define ICON_MEMORY_VIEW  u8"💽"
#define ICON_WATCH_LIST   u8"👁"

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

