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
