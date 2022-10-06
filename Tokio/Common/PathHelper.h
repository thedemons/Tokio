#pragma once
#ifndef TOKIO_COMMON_PATH_HELPER_H
#define TOKIO_COMMON_PATH_HELPER_H

#include <string>

namespace Tokio
{
std::string GetFileExtension(const std::string& fileName) noexcept;
std::wstring GetFileExtension(const std::wstring& fileName) noexcept;

std::string PathAppend(const std::string& path, std::string trail) noexcept;
std::wstring PathAppend(const std::wstring& path, std::wstring trail) noexcept;

std::string PathParent(std::string path, unsigned int level = 1) noexcept;
std::wstring PathParent(std::wstring path, unsigned int level = 1) noexcept;

std::string PathGetTrail(const std::string& path, unsigned int level = 1) noexcept;
std::wstring PathGetTrail(const std::wstring& path, unsigned int level = 1) noexcept;

}
#endif // !TOKIO_COMMON_PATH_HELPER_H

