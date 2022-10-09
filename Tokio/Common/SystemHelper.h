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
#ifndef TOKIO_COMMON_SYSTEM_HELPER_H
#define TOKIO_COMMON_SYSTEM_HELPER_H

#include "TypeDef.h"
#include "Exception.h"

#include <string>
#include <map>
#include <vector>

namespace Tokio
{


_NODISCARD ProcessEntry GetProcess(PID pid) EXCEPT;

// get a list of processes
_NODISCARD std::map<PID, ProcessEntry> GetAllProcess() EXCEPT;

// Get a list of windows, includes their titles, classname and pid
_NODISCARD std::vector<WindowEntry>GetAllWindows() EXCEPT;

// open file explorer and select the file
bool OpenFileInExplorer(const std::wstring& filePath) noexcept;

bool ClipboardCopy(const std::wstring& text) noexcept;
bool ClipboardCopy(const std::string& text) noexcept;
bool ClipboardCopy(unsigned long long number, bool isHex) noexcept;
}
#endif // !TOKIO_COMMON_SYSTEM_HELPER_H

