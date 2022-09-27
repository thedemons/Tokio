#pragma once
#ifndef TOKIO_SETTINGS_HPP
#define TOKIO_SETTINGS_HPP

namespace settings
{
enum class ThemeType
{
	Dark,
	Light,
	VSDark,
	Custom,
};

class ThemeData : public Serializable
{
protected:

	void Serialize(SerializeStream& stream) override
	{
		stream << type << customThemePath;
	}

	void Deserialize(SerializeStream& stream) override
	{
		stream >> type >> customThemePath;
	}

public:
	ThemeType type;
	std::wstring customThemePath;
};


class SettingData : public Serializable
{
public:
	friend class Serializable;
	friend class SerializeStream;

	void Serialize(SerializeStream& stream) override
	{
		//stream << theme;
	}

	void Deserialize(SerializeStream& stream) override
	{
		//stream >> theme;
	}
public:
	ThemeData theme;

};
}
#endif