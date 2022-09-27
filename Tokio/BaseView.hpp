#pragma once
class BaseView
{
public:
	inline virtual std::string Title() = 0;
	inline virtual bool isClosable() = 0;
	virtual void Render(bool* bOpen) = 0;
};
