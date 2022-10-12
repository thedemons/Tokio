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
#ifndef TOKIO_IMGUI_CUSTOM_TOKENIZED_TEXT_H
#define TOKIO_IMGUI_CUSTOM_TOKENIZED_TEXT_H

#ifndef IMGUI_API
#include "GUI/ImGui/imgui.h"
#endif // ! IMGUI_API

#include <string>
#include <vector>

struct IMGUI_API ImGuiWindow;
struct ImGuiContext;
struct IMGUI_API ImRect;

namespace ImGui
{
class TokenizedText
{
public:
    struct Token
    {
        std::string text;
        ImU32 color;
        Token(const char* text, ImU32 color) : text(text), color(color) {}
        Token(const std::string& string, ImU32 color) : text(string), color(color) {}

        template <size_t Size>
        Token(const char(&text)[Size], ImU32 color) : text(text, Size), color(color) {}
    };


private:
    std::vector<Token> m_tokens;
    mutable size_t m_cstrCachedSize = 0;
    mutable std::string m_cstr;

    // MODIFIED ImGui::TextEx
    void RenderToken(ImGuiContext& g, ImGuiWindow* window, ImFont* font, float fontSize, const Token& token) const;

public:
    // reserved size for the vector
    TokenizedText(size_t reserved = 0);

    TokenizedText(const char* text, ImU32 color);
    TokenizedText(const std::string& string, ImU32 color);
    TokenizedText(ImU32 color, const char* fmt, ...);

    TokenizedText(const char* text, ImVec4 color);
    TokenizedText(const std::string& string, ImVec4 color);
    TokenizedText(ImVec4 color, const char* fmt, ...);

    // prefer to use ImU32 for performance
    void push_back(const char* text, ImU32 color);
    void push_back(const std::string& string, ImU32 color);
    void push_back(ImU32 color, const char* fmt, ...);

    // prefer to use ImU32 for performance
    void push_back(const char* text, ImVec4 color);
    void push_back(const std::string& string, ImVec4 color);
    void push_back(ImVec4 color, const char* fmt, ...);

    // render in the window, same as ImGui::Text()
    void Render(ImFont* font = nullptr) const;

    // render in the drawlist, same as ImDrawList->AddText()
    void Render(ImDrawList* drawList, const ImVec2& pos, ImFont* font = nullptr, float fontSize = 0.f) const;

    // render in the drawlist, same as ImDrawList->AddText()
    void Render(ImDrawList* drawList, const ImVec2& pos_min, const ImVec2& pos_max, const ImRect* clip_rect, ImFont* font = nullptr, float fontSize = 0.f, const ImVec2& align = ImVec2(0.f,0.f), const ImVec2* text_size_if_known = nullptr) const;

    template <size_t Size>
    TokenizedText(const char(&text)[Size], ImU32 color)
    {
        m_tokens.emplace_back(text, color);
    }

    _NODISCARD _CONSTEXPR20 const char* c_str() const noexcept
    {
        if (m_cstrCachedSize != m_tokens.size())
        {
            m_cstr.clear();
            m_cstrCachedSize = m_tokens.size();
            for (const Token& token : m_tokens) m_cstr += token.text;
        }

        return m_cstr.c_str();
    }

    // move the other tokenizedText to this
    _CONSTEXPR20 void move(TokenizedText& other)
    {
        m_tokens = std::move(other.m_tokens);
    }

    template <size_t Size>
    _CONSTEXPR20 void push_back(const char(&text)[Size], ImU32 color)
    {
        m_tokens.emplace_back(text, color);
    }

    _NODISCARD _CONSTEXPR20 size_t size() const noexcept
    {
        return m_tokens.size();
    }

    // allow modifying of a token color, use with cautions
    _NODISCARD _CONSTEXPR20 ImU32& color(size_t index)
    {
        return m_tokens[index].color;
    }

    _CONSTEXPR20 TokenizedText& operator+=(const TokenizedText& b) 
    {
        if (b.size() > 0)
        {
            m_tokens.insert(m_tokens.end(), b.m_tokens.begin(), b.m_tokens.end());
        }

        return *this;
    }

    _CONSTEXPR20 TokenizedText& operator+=(const std::string& b) 
    {
        push_back(b, ImGui::GetColorU32(ImGuiCol_Text));
        return *this;
    }

    template <size_t Size>
    _CONSTEXPR20 TokenizedText& operator+=(const char(&b)[Size])
    {
        push_back(b, ImGui::GetColorU32(ImGuiCol_Text));
        return *this;
    }

    _CONSTEXPR20 TokenizedText& operator+=(const char* b) 
    {
        push_back(b, ImGui::GetColorU32(ImGuiCol_Text));
        return *this;
    }


    _CONSTEXPR20 TokenizedText operator+(const TokenizedText& b)
    {
        TokenizedText c = *this;

        if (b.size() > 0)
        {
            c.m_tokens.insert(c.m_tokens.end(), b.m_tokens.begin(), b.m_tokens.end());
        }

        return c;
    }

    _CONSTEXPR20 TokenizedText operator+(const std::string& b)
    {
        TokenizedText c = *this;
        push_back(b, ImGui::GetColorU32(ImGuiCol_Text));

        return c;
    }

    template <size_t Size>
    _CONSTEXPR20 TokenizedText operator+(const char(&b)[Size])
    {
        TokenizedText c = *this;
        push_back(b, ImGui::GetColorU32(ImGuiCol_Text));

        return c;
    }

    _CONSTEXPR20 TokenizedText operator+(const char* b)
    {
        TokenizedText c = *this;
        push_back(b, ImGui::GetColorU32(ImGuiCol_Text));

        return c;
    }

    // insert a space if the last token doesn't end with a space
    _CONSTEXPR20 void append_space(const TokenizedText& b)
    {
        if (b.size() == 0) return;

        if (size() > 0)
        {
            auto& lastToken = m_tokens.back();
            if (lastToken.text.back() != ' ') lastToken.text += ' ';
        }

        m_tokens.insert(m_tokens.end(), b.m_tokens.begin(), b.m_tokens.end());
    }

    _CONSTEXPR20 void clear() noexcept
    {
        m_tokens.clear();
    }

    _NODISCARD ImVec2 CalcSize(ImFont* font = nullptr, float size = 0.f) const;
};
}

#endif // !TOKIO_IMGUI_CUSTOM_TOKENIZED_TEXT_H
