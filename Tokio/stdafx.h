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

#include "Serialize.hpp"
#include "common_err.hpp"
#include "settings.hpp"

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