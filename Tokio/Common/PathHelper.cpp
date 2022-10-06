#include "stdafx.h"
#include "PathHelper.h"

namespace Tokio
{

std::string GetFileExtension(const std::string& fileName) noexcept
{
	for (size_t i = fileName.size() - 1; i > 0; i--)
	{
		if (fileName[i] == '\\') break;
		else if (fileName[i] == '.')
		{
			return fileName.substr(i + 1);
		}
	}
	return std::string();
}
std::wstring GetFileExtension(const std::wstring& fileName) noexcept
{
	for (size_t i = fileName.size() - 1; i > 0; i--)
	{
		if (fileName[i] == L'\\') break;
		else if (fileName[i] == L'.')
		{
			return fileName.substr(i + 1);
		}
	}
	return std::wstring();
}

std::string PathAppend(const std::string& path, std::string trail) noexcept
{
	return (path.back() != '\\') ? (path + '\\' + trail) : (path + trail);
}

std::wstring PathAppend(const std::wstring& path, std::wstring trail) noexcept
{
	return (path.back() != L'\\') ? (path + L'\\' + trail) : (path + trail);
}

std::string PathParent(std::string path, unsigned int level) noexcept
{
	// remove trailing slash
	if (path.back() == '\\') path = path.substr(0, path.size() - 1);

	for (unsigned int i = 0; i < level; i++)
	{
		path = path.substr(0, path.find_last_of('\\'));
	}
	return path;
}

std::wstring PathParent(std::wstring path, unsigned int level) noexcept
{
	// remove trailing slash
	if (path.back() == L'\\') path = path.substr(0, path.size() - 1);

	for (unsigned int i = 0; i < level; i++)
	{
		path = path.substr(0, path.find_last_of(L'\\'));
	}
	return path;
}

std::string PathGetTrail(const std::string& path, unsigned int level) noexcept
{
	std::string tempPath = PathParent(path, level);
	return path.substr(tempPath.size() + 1);
}

std::wstring PathGetTrail(const std::wstring& path, unsigned int level) noexcept
{
	std::wstring tempPath = PathParent(path, level);
	return path.substr(tempPath.size() + 1);
}

}