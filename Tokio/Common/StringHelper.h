#pragma once
#ifndef TOKIO_COMMON_STRING_HELPER_H
#define TOKIO_COMMON_STRING_HELPER_H

#include <string>

namespace Tokio
{
std::string String(const std::wstring& str) noexcept;
std::wstring String(const std::string& str) noexcept;

std::wstring StringLower(std::wstring str) noexcept;
std::string StringLower(std::string str) noexcept;

std::wstring StringUpper(std::wstring str) noexcept;
std::string StringUpper(std::string str) noexcept;

}
#endif // !TOKIO_COMMON_STRING_HELPER_H

