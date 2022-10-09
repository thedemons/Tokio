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

