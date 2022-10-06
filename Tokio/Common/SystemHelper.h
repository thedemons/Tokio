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

