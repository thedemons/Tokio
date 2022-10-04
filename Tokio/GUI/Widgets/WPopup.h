#pragma once
#ifndef TOKIO_GUI_WIDGETS_POPUP
#define TOKIO_GUI_WIDGETS_POPUP

#include "imgui.hpp"
#include "WidgetsFlags.hpp" 

namespace Widgets
{
class Popup
{
public:

	typedef void (*LPRENDER_CALLBACK)(Popup* popup, void* OpenUserData, void* UserData);

	struct Desc
	{
		ImGuiID	id = 0u;		// if no id were specify, an id will be generated automatically
		std::string name;		// used for modal popups

		ImVec2 initialSize = { 0.f, 0.f };  // initial size on first open

		// popup window flags
		ImGuiWindowFlags Flags = 
			ImGuiWindowFlags_NoTitleBar			|
			ImGuiWindowFlags_NoSavedSettings	|
			ImGuiWindowFlags_AlwaysAutoResize	;

		// Flag when the popup is openned
		ImGuiPopupFlags OpenFlags =
			ImGuiPopupFlags_None;

		// Flags of this widget
		PopupFlags WidgetFlags = PopupFlags::None;

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
	bool m_nextOpen = false;

public:
	void Setup(const Desc& desc);

	void Render();

	_CONSTEXPR20 void Open(void* OpenUserData = nullptr)
	{
		SetOpenUserData(OpenUserData);
		m_nextOpen = true;
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

#endif // !TOKIO_GUI_WIDGETS_POPUP