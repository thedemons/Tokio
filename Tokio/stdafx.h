#pragma once
#pragma warning (push, 3)

#include <map>

#include <array>
#include <vector>

#include <type_traits>
#include <variant>

#include <string>
#include <fstream>
#include <sstream>

#include <filesystem>
#include <iostream>

#include <chrono>
#include <functional>


#define UNUSED(...) (void)__VA_ARGS__;

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

#pragma warning (pop)

