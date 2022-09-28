#pragma once
#include "ThirdParty/Result.hpp"

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <cwctype>

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#ifdef _DEBUG
#define DPRINT(...) printf(__VA_ARGS__)
#else
#define DPRINT(...) // __VA_ARGS__
#endif // _DEBUG

#include "Serialize.hpp"
#include "Settings.hpp"
#include "common_err.hpp"

#define SafeResult(...) cpp::result<__VA_ARGS__, common::err>

#define WINAPI_FAILIFN(result, code) \
if (!(result)) { \
	return cpp::fail(common::err(common::errcode::code, true)); \
}

#define RESULT_FAILIFN(result, code) \
if (!(result)) { \
	return cpp::fail(common::err(common::errcode::code)); \
}

#define WINAPI_FAILIFN_NM(result) \
if (!(result)) { \
	return cpp::fail(common::err(common::errcode::NoMessage, true)); \
}

#define RESULT_FAILIFN_NM(result) \
if (!(result)) { \
	return cpp::fail(common::err(common::errcode::NoMessage)); \
}