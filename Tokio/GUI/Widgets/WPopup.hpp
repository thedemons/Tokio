#pragma once
namespace Widgets
{
class Popup
{
public:

	typedef void (*LPRENDER_CALLBACK)(Popup* popup, void* OpenUserData, void* UserData);

	struct Desc
	{
		ImGuiID	id = 0u; // if no id were specify, an id will be generated automatically

		// popup window flags
		ImGuiWindowFlags Flags = 
			ImGuiWindowFlags_NoTitleBar			|
			ImGuiWindowFlags_NoSavedSettings	|
			ImGuiWindowFlags_AlwaysAutoResize	;

		// Flag when the popup is openned
		ImGuiPopupFlags OpenFlags =
			ImGuiPopupFlags_None;


		// UserData is set when setting up the description
		// OpenUserData is whatever passed to ::Open() or ::SetOpenUserData()
		LPRENDER_CALLBACK RenderCallback = nullptr;

		void* RenderUserData = nullptr;

		// Set this in ::Open or ::SetOpenUserData,
		// it doesn't take effect when setting up the description
		void* OpenUserData = nullptr;			
	};

private:
	// popup description
	Desc m_desc;
	bool m_isOpen = false;
	bool m_closePopupQueue = false;

public:
	void Setup(const Desc& desc)
	{
		// must set the render callback
		assert(desc.RenderCallback != nullptr);

		m_desc = desc;
		if (m_desc.id == 0) m_desc.id = GUIUtils::GetUniqueID(this);
	}

	void Render()
	{
		m_isOpen = ImGui::BeginPopupEx(m_desc.id, m_desc.Flags);

		if (m_isOpen)
		{
			m_desc.RenderCallback(this, m_desc.OpenUserData, m_desc.RenderUserData);

			if (m_closePopupQueue)
			{
				ImGui::CloseCurrentPopup();
				m_closePopupQueue = false;
			}
			ImGui::EndPopup();
		}

		m_closePopupQueue = false;
	}

	_CONSTEXPR20 void Open(void* OpenUserData = nullptr)
	{
		SetOpenUserData(OpenUserData);
		ImGui::OpenPopupEx(m_desc.id, m_desc.OpenFlags);
	}

	_CONSTEXPR20 void Close()
	{
		m_closePopupQueue = true;
	}

	_CONSTEXPR20 void SetOpenUserData(void* OpenUserData)
	{
		m_desc.OpenUserData = OpenUserData;
	}

	_CONSTEXPR20 bool IsOpen() const
	{
		return m_isOpen;
	}

};
}