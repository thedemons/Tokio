#pragma once
#include "ThirdParty/Result.hpp"

#include <map>
#include <vector>
#include <type_traits>
#include <variant>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <cwctype>
#include <ShlObj.h>
#include <comdef.h>

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#ifdef _DEBUG
#define DPRINT(...) printf(__VA_ARGS__)
#define THROW_CPP_ERROR 
//#define THROW_CPP_ERROR __debugbreak();
#else
#define THROW_CPP_ERROR 
#define DPRINT(...) // __VA_ARGS__
#endif // _DEBUG

#include "Serialize.hpp"
#include "Settings.hpp"
#include "common_err.hpp"

#define SafeResult(...) cpp::result<__VA_ARGS__, common::err>

#define WINAPI_FAILIFN(result, code) \
if (!(result)) { \
	THROW_CPP_ERROR \
	return cpp::fail(common::err(common::errcode::code, common::errtype::WinAPI, GetLastError())); \
}

#define HRESULT_FAILIFN(result, code) \
if (result != S_OK) { \
	THROW_CPP_ERROR \
	return cpp::fail(common::err(common::errcode::code, common::errtype::HRESULT, result)); \
}

#define RESULT_FAILIFN(result, code) \
if (!(result)) { \
	THROW_CPP_ERROR \
	return cpp::fail(common::err(common::errcode::code)); \
}

#define WINAPI_FAILIFN_NM(result) \
if (!(result)) { \
	return cpp::fail(common::err(common::errcode::NoMessage, common::errtype::WinAPI, GetLastError())); \
}

#define HRESULT_FAILIFN_NM(result) \
if (result != S_OK) { \
	return cpp::fail(common::err(common::errcode::NoMessage, common::errtype::HRESULT, result)); \
}

#define RESULT_FAILIFN_NM(result) \
if (!(result)) { \
	return cpp::fail(common::err(common::errcode::NoMessage)); \
}

#define RESULT_FAILIFN_PASS(result, error) \
if (!(result)) { \
	return cpp::fail(error); \
}

#define RESULT_FORWARD(result) return cpp::fail(common::err(result.error()))

#define RESULT_THROW(code) return cpp::fail(common::err(common::errcode::code))
#define WINAPI_THROW(code) return cpp::fail(common::err(common::errcode::code, common::errtype::WinAPI))
#define HRESULT_THROW(code) return cpp::fail(common::err(common::errcode::code, common::errtype::HRESULT))


#ifdef _WIN64
constexpr auto UPTR_UNDEFINED = ~0ull;
#else
constexpr auto UPTR_UNDEFINED ~0ul
#endif

#include "common_helper.h"