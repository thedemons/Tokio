#pragma once
#include "stdafx.h"
#include "StringHelper.h"

#include <cwctype>
#include <locale>
#include <codecvt>

namespace Tokio
{
std::string String(const std::wstring& str) noexcept
{
	//setup converter
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	return converter.to_bytes(str);
}

std::wstring String(const std::string& str) noexcept
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

std::wstring StringLower(std::wstring str) noexcept
{
	std::transform(str.begin(), str.end(), str.begin(), std::towlower);
	return str;
}

std::string StringLower(std::string str) noexcept
{
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
	return str;
}

std::wstring StringUpper(std::wstring str) noexcept
{
	std::transform(str.begin(), str.end(), str.begin(), std::towupper);
	return str;
}

std::string StringUpper(std::string str) noexcept
{
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
	return str;
}


}