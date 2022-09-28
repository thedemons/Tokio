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
	inline void Setup(Desc desc)
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

	inline const char* c_str()
	{
		return m_buffer.c_str();
	}

	inline char* data()
	{
		return m_buffer.data();
	}

	inline size_t length()
	{
		return m_buffer.length();
	}

	inline size_t strlen()
	{
		return m_privateData.BufTextLen;
	}

	inline std::string& str()
	{
		return m_buffer;
	}

	// the string without trailing null bytes
	inline std::string str_strip()
	{
		return std::string(m_buffer.c_str(), m_privateData.BufTextLen);
	}
};
}