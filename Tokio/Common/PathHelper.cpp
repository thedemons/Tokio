/*
 * Project Tokio
 * Author: thedemons
 * Github: thedemons/Tokio
 * Made:   With love
 *
 * License: MIT License
 *
 * Copyright(c) 2022 thedemons
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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