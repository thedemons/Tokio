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

class ThemeData
{
public:
	ThemeType type;
	std::wstring customThemePath;
};


class SettingData
{
public:
	ThemeData theme;

};
}
#endif