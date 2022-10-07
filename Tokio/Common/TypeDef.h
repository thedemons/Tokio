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
