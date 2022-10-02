#pragma once
#ifndef TOKIO_GUI_WIDGETS_TEXT_INPUT_H
#define TOKIO_GUI_WIDGETS_TEXT_INPUT_H

#include "imgui.hpp"

namespace Widgets
{
class TextInput
{
public:

	typedef void (*LPEDIT_CALLBACK)(TextInput* tinput, ImGuiInputTextCallbackData* data, void* UserData);
	struct Desc
	{
		std::string Label;
		std::string Hint;
		ImGuiInputTextFlags Flags = ImGuiInputTextFlags_None;

		// Call on modification of the text input content
		LPEDIT_CALLBACK EditCallback = nullptr;
		void* EditUserData = nullptr;
	};

private:
	Desc m_desc;
	std::string m_buffer;
	ImGuiInputTextCallbackData m_privateData;
	bool m_nextFocus = false;

	static int ResizeCallback(ImGuiInputTextCallbackData* data);

public:
	void Setup(const Desc& desc);

	bool Render(const ImVec2& size = { 0.f, 0.f });

	_CONSTEXPR20 const char* c_str() const
	{
		return m_buffer.c_str();
	}

	_CONSTEXPR20 char* data()
	{
		return m_buffer.data();
	}

	_CONSTEXPR20 size_t length() const
	{
		return m_buffer.length();
	}

	_CONSTEXPR20 size_t strlen() const
	{
		return m_privateData.BufTextLen;
	}

	_CONSTEXPR20 std::string& str()
	{
		return m_buffer;
	}

	// the string without trailing null bytes
	_CONSTEXPR20 std::string str_strip() const
	{
		return std::string(m_buffer.c_str(), m_privateData.BufTextLen);
	}

	_CONSTEXPR20 void Focus()
	{
		m_nextFocus = true;
	}

	// set the text buffer to something else
	_CONSTEXPR20 void SetText(const std::string& text)
	{
		m_buffer = text;
	}
	// clear the text buffer
	_CONSTEXPR20 void Clear()
	{
		m_buffer.clear();
	}
};
}

#endif // !TOKIO_GUI_WIDGETS_TEXT_INPUT_H
