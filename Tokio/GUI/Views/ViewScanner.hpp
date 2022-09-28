#pragma once


class ViewScanner : public BaseView
{

public:
	inline std::string Title() override
	{
		return "Memeory Scan";
	}

	inline bool isClosable() override
	{
		return true;
	}

	inline bool defaultOpenMode() override
	{
		return true;
	}

	void Render(bool& bOpen) override
	{
		if (!bOpen) return;

		ImGui::Begin(Title().c_str(), &bOpen);

		
		ImGui::End();
	}
};