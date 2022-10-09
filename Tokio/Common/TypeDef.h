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
#ifndef TOKIO_COMMON_TYPEDEF_H
#define TOKIO_COMMON_TYPEDEF_H

#include <string>

#if !defined(ENVIRONMENT64) && !defined(ENVIRONMENT32)
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif
#endif

typedef unsigned char		byte_t;
typedef unsigned long		dword_t;
typedef unsigned __int64	qword_t;
typedef unsigned __int64	POINTER;


typedef unsigned long		PID;
typedef void*				ProcessHandle;
typedef void*				ModuleHandle;
typedef void*				WindowHandle;


#ifdef ENVIRONMENT64
constexpr auto UPTR_UNDEFINED = ~0ull;
typedef unsigned __int64	dword_ptr;
#else
constexpr auto UPTR_UNDEFINED ~0ul
typedef unsigned long    	dword_ptr;
#endif

struct ProcessEntry
{
	PID pid = 0;
	PID parentPid = 0;
	std::wstring szExe;
};

struct WindowEntry
{
	PID pid = 0;
	WindowHandle hwnd = nullptr;
	std::wstring title;
	std::wstring classname;
};

#endif // !TOKIO_COMMON_TYPEDEF_H
