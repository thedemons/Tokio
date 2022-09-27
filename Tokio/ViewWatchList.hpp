#pragma once


class ViewWatchList : public BaseView
{
public:
	inline std::string Title() override
	{
		return "Watch List";
	}

	inline bool isClosable() override
	{
		return true;
	}

	void Render(bool* bOpen) override
	{
		if (!*bOpen) return;
		ImGui::Begin(Title().c_str(), bOpen);
		ImGui::End();
	}
};