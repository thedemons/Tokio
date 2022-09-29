#pragma once
namespace Widgets
{
class TextInput
{
public:
	struct Desc
	{
		std::string Label;
		std::string Hint;
		ImGuiInputTextFlags Flags = ImGuiInputTextFlags_None;
	};

private:
	Desc m_desc;
	std::string m_buffer;
	ImGuiInputTextCallbackData m_privateData;

	static int ResizeCallback(ImGuiInputTextCallbackData* data)
	{
		TextInput* pThis = static_cast<TextInput*>(data->UserData);

		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			pThis->m_buffer.resize(data->BufSize);
			data->Buf = pThis->m_buffer.data();
		}

		pThis->m_privateData = *data;
		return 0;
	}

public:
	void Setup(const Desc& desc)
	{
		m_desc = desc;
		m_desc.Label = GUIUtils::GetUniqueName(m_desc.Label, this);
	}

	bool Render(const ImVec2& size = {0.f, 0.f})
	{
		return ImGui::InputTextEx(
			m_desc.Label.c_str(),
			m_desc.Hint.c_str(),
			m_buffer.data(),
			static_cast<int>(m_buffer.size()),
			size,
			m_desc.Flags | ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CallbackEdit,
			ResizeCallback,
			this
		);
	}

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
};
}