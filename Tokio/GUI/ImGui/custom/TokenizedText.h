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

    // MODIFIED ImGui::TextEx
    void RenderToken(ImGuiContext& g, ImGuiWindow* window, ImFont* font, float fontSize, const Token& token);

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
    void Render(ImFont* font = nullptr);

    // render in the drawlist, same as ImDrawList->AddText()
    void Render(ImDrawList* drawList, ImVec2 pos, ImFont* font = nullptr);

    template <size_t Size>
    TokenizedText(const char(&text)[Size], ImU32 color)
    {
        m_tokens.emplace_back(text, color);
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
};
}

#endif // !TOKIO_IMGUI_CUSTOM_TOKENIZED_TEXT_H
