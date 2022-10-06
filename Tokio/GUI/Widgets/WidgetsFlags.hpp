#pragma once
#ifndef TOKIO_GUI_WIDGETS_FLAGS_HPP
#define TOKIO_GUI_WIDGETS_FLAGS_HPP

// TODO: FLAGS NEED SERIOUS REWORK

extern "C++" {

    template <size_t S>
    struct _TOKIO_ENUM_FLAG_INTEGER_FOR_SIZE;

    template <>
    struct _TOKIO_ENUM_FLAG_INTEGER_FOR_SIZE<1>
    {
        typedef signed char type;
    };

    template <>
    struct _TOKIO_ENUM_FLAG_INTEGER_FOR_SIZE<2>
    {
        typedef signed short type;
    };

    template <>
    struct _TOKIO_ENUM_FLAG_INTEGER_FOR_SIZE<4>
    {
        typedef signed int type;
    };

    template <>
    struct _TOKIO_ENUM_FLAG_INTEGER_FOR_SIZE<8>
    {
        typedef signed long long type;
    };

    // used as an approximation of std::underlying_type<T>
    template <class T>
    struct _TOKIO_ENUM_FLAG_SIZED_INTEGER
    {
        typedef typename _TOKIO_ENUM_FLAG_INTEGER_FOR_SIZE<sizeof(T)>::type type;
    };

}

#define TOKIO_DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) \
inline constexpr ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) noexcept { return ENUMTYPE(((_TOKIO_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) | ((_TOKIO_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator |= (ENUMTYPE &a, ENUMTYPE b) noexcept { return (ENUMTYPE &)(((_TOKIO_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) |= ((_TOKIO_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline constexpr ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) noexcept { return ENUMTYPE(((_TOKIO_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) & ((_TOKIO_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator &= (ENUMTYPE &a, ENUMTYPE b) noexcept { return (ENUMTYPE &)(((_TOKIO_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) &= ((_TOKIO_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline constexpr ENUMTYPE operator ~ (ENUMTYPE a) noexcept { return ENUMTYPE(~((_TOKIO_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a)); } \
inline constexpr ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) noexcept { return ENUMTYPE(((_TOKIO_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) ^ ((_TOKIO_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator ^= (ENUMTYPE &a, ENUMTYPE b) noexcept { return (ENUMTYPE &)(((_TOKIO_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) ^= ((_TOKIO_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline bool operator!(ENUMTYPE a) { return !static_cast<bool>(a); } \
inline bool operator==(ENUMTYPE a, unsigned int b) { return static_cast<unsigned int>(a) == b; }

namespace Widgets
{

enum class TableFlags : unsigned int
{
	None = 0,
	NoHeader = 1 << 0,

}; TOKIO_DEFINE_ENUM_FLAG_OPERATORS(TableFlags)

enum class PopupFlags : unsigned int
{
	None = 0,
	PopupModal = 1 << 0,		// modal popup
	CloseOnEscape = 1 << 1, // close on ESCAPE key pressed

}; TOKIO_DEFINE_ENUM_FLAG_OPERATORS(PopupFlags)

}
#endif // !TOKIO_GUI_WIDGETS_FLAGS_HPP
