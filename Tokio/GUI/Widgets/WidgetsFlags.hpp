#pragma once
#ifndef TOKIO_GUI_WIDGETS_FLAGS_HPP
#define TOKIO_GUI_WIDGETS_FLAGS_HPP

namespace Widgets
{
#define WIDGET_ENUM_OPERATORS(EnumType) \
DEFINE_ENUM_FLAG_OPERATORS(EnumType) \
inline bool operator!(EnumType a) { return !static_cast<bool>(a); } \
inline bool operator==(EnumType a, UINT b) { return static_cast<UINT>(a) == b; }

enum class TableFlags : UINT
{
	None = 0,
	NoHeader = 1 << 0,

}; WIDGET_ENUM_OPERATORS(TableFlags)

}
#endif // !TOKIO_GUI_WIDGETS_FLAGS_HPP
