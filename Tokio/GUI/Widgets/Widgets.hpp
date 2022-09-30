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