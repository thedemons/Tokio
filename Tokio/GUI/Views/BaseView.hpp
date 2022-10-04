#pragma once
class BaseView
{
protected:
	std::string m_title;

public:
	_CONSTEXPR20 virtual std::string Title() const
	{
		return m_title;
	}

	_CONSTEXPR20 virtual bool isClosable() const
	{
		return true;
	}

	_CONSTEXPR20 virtual bool isDefaultOpen() const
	{
		return true;
	}

	_CONSTEXPR20 virtual bool isAllowMultipleViews() const
	{
		return true;
	}

	virtual void Render(bool& bOpen) = 0;

	virtual void OnAttach(const std::shared_ptr<ProcessData>& targetProcess)
	{
		UNUSED(targetProcess);
	}

	virtual void OnDetach(){}
};
