#pragma once
#ifndef TOKIO_SETTINGS_HPP
#define TOKIO_SETTINGS_HPP

namespace Settings
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
	void Serialize(SerializeStream& stream) const override
	{
		stream << type << customThemePath;
	}

	void Deserialize(SerializeStream& stream) override
	{
		stream >> type >> customThemePath;
	}

public:
	ThemeType type = ThemeType::VSDark;
	std::wstring customThemePath;
};

class SettingData : public Serializable
{
protected:
	void Serialize(SerializeStream& stream) const override
	{
		stream << theme;
	}

	void Deserialize(SerializeStream& stream) override
	{
		stream >> theme;
	}
public:
	ThemeData theme;

	void Load()
	{
		// load from file
		if (FromFile(L"Tokio.dat")) return;

		// default settings
		theme.type = ThemeType::VSDark;
	}

	void Save()
	{
		if (!ToFile(L"Tokio.dat"))
		{
			DPRINT("Cannot save settings!");
		}
	}
};


inline SettingData data;


}
#endif // !TOKIO_SETTINGS_HPP