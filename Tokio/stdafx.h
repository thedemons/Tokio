#pragma once
#include "ThirdParty/Result.hpp"

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

#include <Windows.h>

#include "Serialize.hpp"
#include "common.hpp"
#include "settings.hpp"

#define SafeResult(T) cpp::result<T, common::err>

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