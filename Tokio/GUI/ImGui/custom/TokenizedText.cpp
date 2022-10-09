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
#include "stdafx.h"
#include "TokenizedText.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "GUI/ImGui/imgui_internal.h"

namespace ImGui
{


TokenizedText::TokenizedText(const char* text, ImU32 color)
{
    m_tokens.emplace_back(text, color);
}

TokenizedText::TokenizedText(const std::string& string, ImU32 color)
{
    m_tokens.emplace_back(string, color);
}


TokenizedText::TokenizedText(ImU32 color, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    ImGuiContext& g = *GImGui;
    ImFormatStringV(g.TempBuffer.Data, static_cast<size_t>(g.TempBuffer.Size), fmt, args);

    va_end(args);

    m_tokens.emplace_back(g.TempBuffer.Data, color);
}

TokenizedText::TokenizedText(const char* text, ImVec4 color)
{
    m_tokens.emplace_back(text, ImGui::ColorConvertFloat4ToU32(color));
}

TokenizedText::TokenizedText(const std::string& string, ImVec4 color)
{
    m_tokens.emplace_back(string, ImGui::ColorConvertFloat4ToU32(color));
}

TokenizedText::TokenizedText(ImVec4 color, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    ImGuiContext& g = *GImGui;
    ImFormatStringV(g.TempBuffer.Data, static_cast<size_t>(g.TempBuffer.Size), fmt, args);

    va_end(args);

    m_tokens.emplace_back(g.TempBuffer.Data, ImGui::ColorConvertFloat4ToU32(color));
}

TokenizedText::TokenizedText(size_t reserved)
{
    if (reserved > 0) m_tokens.reserve(reserved);
}

// prefer to use ImU32 for performance
void TokenizedText::push_back(const char* text, ImVec4 color)
{
    m_tokens.emplace_back(text, ImGui::ColorConvertFloat4ToU32(color));
}

void TokenizedText::push_back(const std::string& string, ImVec4 color)
{
    m_tokens.emplace_back(string, ImGui::ColorConvertFloat4ToU32(color));
}

// prefer to use ImU32 for performance
void TokenizedText::push_back(ImVec4 color, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    ImGuiContext& g = *GImGui;
    ImFormatStringV(g.TempBuffer.Data, static_cast<size_t>(g.TempBuffer.Size), fmt, args);

    va_end(args);

    m_tokens.emplace_back(g.TempBuffer.Data, ImGui::ColorConvertFloat4ToU32(color));
}

void TokenizedText::push_back(const char* text, ImU32 color)
{
    m_tokens.emplace_back(text, color);
}

void TokenizedText::push_back(const std::string& string, ImU32 color)
{
    m_tokens.emplace_back(string, color);
}

void TokenizedText::push_back(ImU32 color, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    ImGuiContext& g = *GImGui;
    ImFormatStringV(g.TempBuffer.Data, static_cast<size_t>(g.TempBuffer.Size), fmt, args);

    va_end(args);

    m_tokens.emplace_back(g.TempBuffer.Data, color);
}

// render in the window, same as ImGui::Text()
void TokenizedText::Render(ImFont* font)
{
    if (m_tokens.size() == 0) return;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)  return;

    ImGuiContext& g = *GImGui;
    if (font == nullptr) font = g.Font;

    // we don't call ImGui::SameLine() for the last token
    for (auto iter = m_tokens.begin(); iter != m_tokens.end() - 1; iter++)
    {
        RenderToken(g, window, font, font->FontSize, *iter);
        SameLine(0, 0);
    }

    // render the last token
    RenderToken(g, window, font, font->FontSize, m_tokens.back());
}

// render in the drawlist, same as ImDrawList->AddText()
void TokenizedText::Render(ImDrawList* drawList, ImVec2 pos, ImFont* font, float fontSize)
{
    if (m_tokens.size() == 0) return;

    ImGuiContext& g = *GImGui;
    if (font == nullptr) font = g.Font;
    if (fontSize == 0.f) fontSize = font->FontSize;

    // might implement it in the future
    float wrap_width = 0.f;
    ImVec2 currentPosition = pos;

    for (Token& token : m_tokens)
    {
        const char* text_begin = token.text.c_str();
        const char* text_end = text_begin + token.text.size();

        const ImVec2 text_size = font->CalcTextSizeA(font->FontSize, FLT_MAX, wrap_width, text_begin, text_end, 0);

        ImVec4 clip_rect = drawList->_CmdHeader.ClipRect;
        font->RenderText(drawList, fontSize, currentPosition, token.color, clip_rect, text_begin, text_end, 0.f, false);

        currentPosition.x += text_size.x;
    }
}

// MODIFIED ImGui::TextEx
void TokenizedText::RenderToken(ImGuiContext& g, ImGuiWindow* window, ImFont* font, float fontSize, const Token& token)
{
    const ImVec2 text_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
    const float wrap_pos_x = window->DC.TextWrapPos;
    const bool wrap_enabled = (wrap_pos_x >= 0.0f);

    const char* text_begin = token.text.c_str();
    const char* text_end = text_begin + token.text.size(); // not including NULL byte at the end

    if (token.text.size() <= 2000 || wrap_enabled)
    {
        // Common case
        const float wrap_width = wrap_enabled ? CalcWrapWidthForPos(window->DC.CursorPos, wrap_pos_x) : 0.0f;
        const ImVec2 text_size = CalcTextSize(text_begin, text_end, false, wrap_width);

        ImRect bb(text_pos, text_pos + text_size);
        ItemSize(text_size, 0.0f);
        if (!ItemAdd(bb, 0))
            return;

        // Render (we don't hide text after ## in this end-user function)
        // MODIFIED: shortcut for RenderTextWrappedCustom
        if (text_begin != text_end)
        {
            window->DrawList->AddText(font, fontSize, bb.Min, token.color, text_begin, text_end, wrap_width);
        }
    }
    else
    {
        // Long text!
        // Perform manual coarse clipping to optimize for long multi-line text
        // - From this point we will only compute the width of lines that are visible. Optimization only available when word-wrapping is disabled.
        // - We also don't vertically center the text within the line full height, which is unlikely to matter because we are likely the biggest and only item on the line.
        // - We use memchr(), pay attention that well optimized versions of those str/mem functions are much faster than a casually written loop.
        const char* line = text_begin;
        const float line_height = g.FontSize; // MODIFIED: GetTextLineHeight() removed
        ImVec2 text_size(0, 0);

        // Lines to skip (can't skip when logging text)
        ImVec2 pos = text_pos;
        if (!g.LogEnabled)
        {
            int lines_skippable = (int)((window->ClipRect.Min.y - text_pos.y) / line_height);
            if (lines_skippable > 0)
            {
                int lines_skipped = 0;
                while (line < text_end && lines_skipped < lines_skippable)
                {
                    const char* line_end = static_cast<const char*>(memchr(line, '\n', text_end - line));
                    if (!line_end)
                        line_end = text_end;
                    //if ((flags & ImGuiTextFlags_NoWidthForLargeClippedText) == 0)
                    //    text_size.x = ImMax(text_size.x, CalcTextSize(line, line_end, false).x);
                    line = line_end + 1;
                    lines_skipped++;
                }
                pos.y += lines_skipped * line_height;
            }
        }

        // Lines to render
        if (line < text_end)
        {
            ImRect line_rect(pos, pos + ImVec2(FLT_MAX, line_height));
            while (line < text_end)
            {

                if (!line_rect.Overlaps(window->ClipRect) && !g.LogEnabled) // MODIFIED: IsClippedEx removed
                    break;

                //if (IsClippedEx(line_rect, 0))
                //    break;

                const char* line_end = (const char*)memchr(line, '\n', text_end - line);
                if (!line_end)
                    line_end = text_end;
                text_size.x = ImMax(text_size.x, CalcTextSize(line, line_end).x);

                // MODIFIED: shortcut for RenderText()
                window->DrawList->AddText(font, fontSize, pos, GetColorU32(ImGuiCol_Text), line, line_end);

                line = line_end + 1;
                line_rect.Min.y += line_height;
                line_rect.Max.y += line_height;
                pos.y += line_height;
            }

            // Count remaining lines
            int lines_skipped = 0;
            while (line < text_end)
            {
                const char* line_end = (const char*)memchr(line, '\n', text_end - line);
                if (!line_end)
                    line_end = text_end;
                //if ((flags & ImGuiTextFlags_NoWidthForLargeClippedText) == 0)
                //    text_size.x = ImMax(text_size.x, CalcTextSize(line, line_end).x);
                line = line_end + 1;
                lines_skipped++;
            }
            pos.y += lines_skipped * line_height;
        }
        text_size.y = (pos - text_pos).y;

        ImRect bb(text_pos, text_pos + text_size);
        ItemSize(text_size, 0.0f);
        ItemAdd(bb, 0);
    }
}

_NODISCARD ImVec2 TokenizedText::CalcSize(ImFont* font, float size) const
{
    ImGuiContext& g = *GImGui;
    if (font == nullptr) font = g.Font;
    if (size == 0.f) size = font->FontSize;

    ImVec2 textSize{ 0,0 };
    for (const Token& token : m_tokens)
    {
        ImVec2 calcSize = font->CalcTextSizeA(size, FLT_MAX, 0.f, token.text.c_str(), token.text.c_str() + token.text.size());
        textSize.x += calcSize.x;
        if (calcSize.y > textSize.y) textSize.y = calcSize.y;
    }

    return textSize;
}
}

