#pragma once
#ifndef TOKIO_SETTINGS_HPP
#define TOKIO_SETTINGS_HPP

namespace Settings
{

class Theme : public Serializable
{
protected:
	void Serialize(SerializeStream& stream) const override
	{
		stream << type << customThemePath << disasm;
	}

	void Deserialize(SerializeStream& stream) override
	{
		stream >> type >> customThemePath >> disasm;
	}

public:
	enum class Type
	{
		Dark,
		Light,
		VSDark,
		Custom,
	};
	Type type = Type::VSDark;

	// user-defined theme
	std::wstring customThemePath;

	struct DisasmTheme
	{
		DWORD Invalid;          // An invalid token
		DWORD Delimeter;        // A delimiter character (like `,`, `:`, `+`, `-`, `*`).
		DWORD ParenthesisOpen;  // An open parenthesis character (like '(', `[`, `{`).
		DWORD ParenthesisClose; // A close parenthesis character (like `)`, `]`, `}`).
		DWORD Prefix;           // A prefix literal (like "LOCK", "REP").
		DWORD Mnemonic;         // A mnemonic literal (like "MOV", "VCMPPSD", "LCALL").
		DWORD Register;         // A register literal (like "RAX", "DS", "%ECX").
		DWORD AddressAbs;       // An absolute address literal (like `0x00400000`).
		DWORD AddressRel;       // A relative address literal (like `-0x100`).
		DWORD Displacement;     // A displacement literal (like `0xFFFFFFFF`, `-0x100`, `+0x1234`).
		DWORD Immediate;        // An immediate literal (like `0xC0`, `-0x1234`, `$0x0000`).
		DWORD TypeCast;         // A typecast literal (like `DWORD PTR`).
		DWORD Decorator;        // A decorator literal (like "Z", "1TO4").
		DWORD Literal;          // A symbol literal

		// other color
		DWORD Address;          // regular address color
		DWORD Bytes;            // the bytes displayed in the bytes column
		DWORD Module;           // module color
		DWORD Function;         // Function color

		DWORD mneCall;          // Function color
		DWORD mneSyscall;       // Function color
		DWORD mneJump;          // Function color
		DWORD mneJumpCondition; // Function color
		DWORD mneReturn;        // Function color
	} disasm{ 0 };
};

// doesn't need to inherite Serializable since basically this is a struct with no dynamic-size value
struct Disasm
{
	enum class FormatStyle : UINT
	{
		ATT,		// AT&T format
		Intel,		// Intel format
		MASM,		// MASM format
	};

	FormatStyle style = FormatStyle::Intel;

	bool bShowOperandSize = false;
	bool bShowOperanSegment = false;
	bool bShowDetailedPerfix = false;

	bool bUppercasePrefixes = false;
	bool bUppercaseTypeCast = false;
	bool bUppercaseMnemonic = false;
	bool bUppercaseRegister = false;
	bool bUppercaseDecorator = false;

	char cDecPrefix[10]{ 0 };
	char cDecSuffix[10]{ 0 };

	bool bUppercaseHex = true;
	bool bAutoHexPadding = false;
	bool bShowHexLeadingZero = false;
	char cHexPrefix[10]{};
	char cHexSuffix[10] = {'h', '\x00'};
};


class SettingData : public Serializable
{
protected:
	void Serialize(SerializeStream& stream) const override
	{
		stream << theme << disasm;
	}

	void Deserialize(SerializeStream& stream) override
	{
		stream >> theme >> disasm;
	}
public:
	Theme theme;
	Disasm disasm;

	void Load()
	{
		// load from file
		if (FromFile(L"Tokio.dat")) return;

		// default settings
		theme.type = Theme::Type::VSDark;
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