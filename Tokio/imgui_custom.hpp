#pragma once
#ifndef TOKIO_IMGUI_CUSTOM_HPP
#define TOKIO_IMGUI_CUSTOM_HPP
#include <string>

#ifndef IMGUI_CUSTOM_FONT_START_COLOR
#include "GUI/ImGui/imconfig.h"
#endif // !IMGUI_API

class ImGuiCustomString
{
private:
    size_t c_size;
    const char* c_str;
    std::string c_custom;

public:
    ImGuiCustomString(const char* c_str, size_t size) :
        c_str(c_str), c_size(size)
    {
        // 1 byte IMGUI_CUSTOM_FONT_START_COLOR
        // 4 bytes DWORD color
        // c_size bytes string
        // 1 byte IMGUI_CUSTOM_FONT_END_COLOR
        // 6 bytes in TOTAL
        c_custom.resize(c_size + 6);
    }

    ImGuiCustomString(const char* c_str) :
        c_str(c_str), c_custom(c_str)
    {
        c_size = c_custom.size();
        c_custom.resize(c_size + 6);
    }

    // warning! if the string is deleted before you
    // call the operator(), it will crash
    ImGuiCustomString(const std::string& str) :
        c_str(str.c_str()), c_size(str.size())
    {
        c_custom.resize(c_size + 6);
    }

    std::string operator()(unsigned long color)
    {
        char* buf = c_custom.data();

        buf[0] = IMGUI_CUSTOM_FONT_START_COLOR;
        buf[c_size + 5] = IMGUI_CUSTOM_FONT_END_COLOR;

        *(unsigned long*)&buf[1] = color;

        // sanitize null byte
        for (size_t i = 1; i < 5; i++)
            if (buf[i] == 0x00) buf[i] = 0x01;

        // copy the original string
        memcpy(buf + 5, c_str, c_size);

        return c_custom;
    }
};

// Custom colored text rendering, see imconfig.h for explaination
inline ImGuiCustomString operator "" _c(const char* c_str, size_t size)
{
    return ImGuiCustomString(c_str, size);
}
#endif // !TOKIO_IMGUI_CUSTOM_HPP