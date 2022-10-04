#pragma once
#ifndef TOKIO_SETTINGS_HPP
#define TOKIO_SETTINGS_HPP

#include "imgui.hpp"
#include "Serialize.hpp"
#include "Engine/EngineDef.h"
#include "GUI/ShortcutKey.h"

namespace Settings
{
#define SETTINGS_VERSION 0x100


class Theme : public Serializable
{
protected:
	void Serialize(SerializeStream& stream) const override;
	void Deserialize(SerializeStream& stream) override;
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
		ImU32 Invalid;              // An invalid token
		ImU32 Delimeter;            // A delimiter character (like `,`, `:`, `+`, `-`, `*`).
		ImU32 ParenthesisOpen;      // An open parenthesis character (like '(', `[`, `{`).
		ImU32 ParenthesisClose;     // A close parenthesis character (like `)`, `]`, `}`).
		ImU32 Prefix;               // A prefix literal (like "LOCK", "REP").
		ImU32 Mnemonic;             // A mnemonic literal (like "MOV", "VCMPPSD", "LCALL").
		ImU32 Register;             // A register literal (like "RAX", "DS", "%ECX").
		ImU32 AddressAbs;           // An absolute address literal (like `0x00400000`).
		ImU32 AddressRel;           // A relative address literal (like `-0x100`).
		ImU32 Displacement;         // A displacement literal (like `0xFFFFFFFF`, `-0x100`, `+0x1234`).
		ImU32 Immediate;            // An immediate literal (like `0xC0`, `-0x1234`, `$0x0000`).
		ImU32 TypeCast;             // A typecast literal (like `DWORD PTR`).
		ImU32 Decorator;            // A decorator literal (like "Z", "1TO4").
		ImU32 Literal;              // A symbol literal

		                            // other color
		ImU32 Address;              // regular address color
		ImU32 Bytes;                // the bytes displayed in the bytes column
		ImU32 Module;               // module color
		ImU32 Function;             // Function color
		ImU32 String;	            // string color for comments
		ImU32 Xref;				    // cross-reference
		ImU32 ReferenceHighLight;	// highlight color for references

		ImU32 mneCall;          
		ImU32 mneSyscall;       
		ImU32 mneJump;          
		ImU32 mneJumpCondition; 
		ImU32 mneReturn;       
		ImU32 mneInt3;        
	} disasm{ 0 };
};

// doesn't need to inherite Serializable since basically this is a struct with no dynamic-size value
struct Disasm
{
	enum class FormatStyle : unsigned int
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

struct ShortCuts
{
	ShortcutKey DisasmCopyAddress;
	ShortcutKey DisasmGoToAdress;
	ShortcutKey DisasmGoToReference;
	ShortcutKey DisasmFollowInstruction;
	ShortcutKey DisasmAddToWatchList;
	ShortcutKey DisasmOpenInMemoryView;
	
	ShortcutKey SymListCopyAddress;
	ShortcutKey SymListRefresh;
	ShortcutKey SymListAddToWatchList;
	ShortcutKey SymListOpenInMemoryView;
	ShortcutKey SymListOpenInDisassembler;

	ShortcutKey AttachProcAttach;
	ShortcutKey AttachProcOpenFileLocation;
	ShortcutKey AttachProcSuspendProcess;
	ShortcutKey AttachProcKillProcess;
};

extern Theme theme;
extern Disasm disasm;
extern ShortCuts shortcuts;

void Load();

_NODISCARD _CONSTEXPR20 DWORD GetDisasmColor(DisasmOperandType tokenType) noexcept
{
	static const std::map<DisasmOperandType, DWORD> mapColor = {
		{DisasmOperandType::Invalid         , theme.disasm.Invalid},
		{DisasmOperandType::Delimeter       , theme.disasm.Delimeter},
		{DisasmOperandType::ParenthesisOpen , theme.disasm.ParenthesisOpen},
		{DisasmOperandType::ParenthesisClose, theme.disasm.ParenthesisClose},
		{DisasmOperandType::Prefix          , theme.disasm.Prefix},
		{DisasmOperandType::Mnemonic        , theme.disasm.Mnemonic},
		{DisasmOperandType::Register        , theme.disasm.Register},
		{DisasmOperandType::AddressAbs      , theme.disasm.AddressAbs},
		{DisasmOperandType::AddressRel      , theme.disasm.AddressRel},
		{DisasmOperandType::Displacement    , theme.disasm.Displacement},
		{DisasmOperandType::Immediate       , theme.disasm.Immediate},
		{DisasmOperandType::TypeCast        , theme.disasm.TypeCast},
		{DisasmOperandType::Decorator       , theme.disasm.Decorator},
		{DisasmOperandType::Literal         , theme.disasm.Literal},

		{DisasmOperandType::mneCall         , theme.disasm.mneCall},
		{DisasmOperandType::mneSyscall      , theme.disasm.mneSyscall},
		{DisasmOperandType::mneJump         , theme.disasm.mneJump},
		{DisasmOperandType::mneJumpCondition, theme.disasm.mneJumpCondition},
		{DisasmOperandType::mneReturn       , theme.disasm.mneReturn},
		{DisasmOperandType::mneInt3         , theme.disasm.mneInt3},
	};

	auto find = mapColor.find(tokenType);
	if (find == mapColor.end()) return theme.disasm.Invalid;

	return find->second;
}


}
#endif // !TOKIO_SETTINGS_HPP