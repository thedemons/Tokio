/*
 * Project Tokio
 * Author: thedemons
 * Github: thedemons/Tokio
 * Made:   With love
 *
 * License: MIT License
 *
 * Copyright(c) 2022 thedemons
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#ifndef TOKIO_SETTINGS_HPP
#define TOKIO_SETTINGS_HPP

#include "imgui.hpp"
#include "Serialize.hpp"

#include "GUI/ShortcutKey.h"
#include "GUI/ImGui/custom/TokenizedText.h"

#include "Common/TypeDef.h"
#include "Engine/EngineDef.h"

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
		Custom,
	};
	Type type = Type::Dark;

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
		ImU32 Module;               // module color, example: kernel32.dll
		ImU32 Symbol;               // symbol color, example: ntdll."NtProtectVirtualMemory"
		ImU32 Subroutine;           // subroutine color, example: "sub_7ffe71ab8c
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

	struct FontData
	{
		float fontSize = 17.f;
		float fontMonoSize = 14.f;
	} font;

	struct IconData1
	{
		ImU32 save               = 0xFFAF60;
		ImU32 copy               = 0xE3E3E3;
		ImU32 goto_address       = 0xFFB368;
		ImU32 follow_address     = 0xFFD766;
		ImU32 find_references    = 0xFFD766;
		ImU32 switch_mode        = 0xFFD766;
		ImU32 open_file_location = 0x6AD9FF;
		ImU32 freeze             = 0xFFD766;
		ImU32 power_off          = 0x6B62EE;
		ImU32 refresh            = 0xE0E0E0;

		ImU32 attach_proc        = 0xDD963A;
		ImU32 settings           = 0x909090;
		ImU32 symbol_list        = 0xF2EE21;
		ImU32 memory_view        = 0xFFD766;
		ImU32 memory_scan        = 0xFFD766;
		ImU32 watch_list         = 0x6C62EF;
		ImU32 disassembler       = 0x076004;
		ImU32 pseudo_code        = 0xE86071;
	}iconColor1;

	struct IconData2
	{
		ImU32 save               = 0xFFAF60;
		ImU32 copy               = 0xE3E3E3;
		ImU32 goto_address       = 0xFFB368;
		ImU32 follow_address     = 0xFFD766;
		ImU32 find_references    = 0xFFD766;
		ImU32 switch_mode        = 0xFFD766;
		ImU32 open_file_location = 0x6AD9FF;
		ImU32 freeze             = 0xFFD766;
		ImU32 power_off          = 0x6B62EE;
		ImU32 refresh            = 0xE0E0E0;

		ImU32 attach_proc        = 0x62D6FF;
		ImU32 settings           = 0xE0E0E0;
		ImU32 symbol_list        = 0xCECECE;
		ImU32 memory_view        = 0xFFD766;
		ImU32 memory_scan        = 0xFFD766;
		ImU32 watch_list         = 0xE3E3E3;
		ImU32 disassembler       = 0x39C2EA;
		ImU32 pseudo_code        = 0xE86071;
	}iconColor2;

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

struct General
{
	bool bEnableVsync = true;
};

struct ShortCuts
{
	ShortcutKey DisasmCopyAddress;
	ShortcutKey DisasmGoToAdress;
	ShortcutKey DisasmGoToReference;
	ShortcutKey DisasmFollowInstruction;
	ShortcutKey DisasmAddToWatchList;
	ShortcutKey DisasmOpenInMemoryView;
	ShortcutKey DisasmSwitchMode;
	ShortcutKey DisasmDecompile;
	
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
extern General general;
extern Disasm disasm;
extern ShortCuts shortcuts;

void Load();

_NODISCARD _CONSTEXPR20 dword_t GetDisasmColor(DisasmOperandType tokenType) noexcept
{
	// we use an array instead of std::map for a better performance
	static const dword_t arrayColor[] = {
		theme.disasm.Invalid,
		0x00000000,				// white space
		theme.disasm.Delimeter,
		theme.disasm.ParenthesisOpen,
		theme.disasm.ParenthesisClose,
		theme.disasm.Prefix,
		theme.disasm.Mnemonic,
		theme.disasm.Register,
		theme.disasm.AddressAbs,
		theme.disasm.AddressRel,
		theme.disasm.Displacement,
		theme.disasm.Immediate,
		theme.disasm.TypeCast,
		theme.disasm.Decorator,
		theme.disasm.Literal,

		theme.disasm.mneCall,
		theme.disasm.mneSyscall,
		theme.disasm.mneJump,
		theme.disasm.mneJumpCondition,
		theme.disasm.mneReturn,
		theme.disasm.mneInt3,
	};

	size_t index = static_cast<size_t>(tokenType);

	if (!(index >= 0 && index < (sizeof(arrayColor) / sizeof(dword_t)) ))
	{
		assert(false && "Why do we have an invalid tokenType?");
		return theme.disasm.Invalid;
	}

	return arrayColor[index];
}


}
#endif // !TOKIO_SETTINGS_HPP