
#ifndef TOKIO_COMMON_HPP
#define TOKIO_COMMON_HPP

#ifdef _DEBUG
#define DPRINT(...) printf(__VA_ARGS__)
#define THROW_CPP_ERROR 
//#define THROW_CPP_ERROR __debugbreak();
#else
#define THROW_CPP_ERROR 
#define DPRINT(...) // __VA_ARGS__
#define assert(...) // __VA_ARGS__
#endif // _DEBUG

#include "Serialize.hpp"
#include "Settings.hpp"

// Tokio is designed to be x64 to avoid conflicts
// Not sure if we can build as x86 without any changes 
#ifdef _WIN64
constexpr auto UPTR_UNDEFINED = ~0ull;
#else
constexpr auto UPTR_UNDEFINED ~0ul
#endif

#include "common_result.hpp"
#include "common_helper.h"


#endif // !TOKIO_COMMON_HPP