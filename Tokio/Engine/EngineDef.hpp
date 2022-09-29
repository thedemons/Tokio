#pragma once

typedef unsigned __int64 POINTER;

// PE Header structs ===================================================

// PE_SECTION_HEADER* pFirstSectionHeader = 
// &PE_NT_HEADER->OptionalHeader + PE_NT_HEADER->FileHeader.SizeOfOptionalHeader
struct PE_SECTION_HEADER
{
	BYTE		Name[IMAGE_SIZEOF_SHORT_NAME];
	union
	{
		DWORD   PhysicalAddress;
		DWORD   VirtualSize;
	} Misc;
	DWORD		VirtualAddress;
	DWORD		SizeOfRawData;
	DWORD		PointerToRawData;
	DWORD		PointerToRelocations;
	DWORD		PointerToLinenumbers;
	WORD		NumberOfRelocations;
	WORD		NumberOfLinenumbers;
	DWORD		Characteristics;
};

// PE_FILE_HEADER = PE_NT_HEADER->FileHeader
struct PE_FILE_HEADER
{
	WORD		Machine;
	WORD		NumberOfSections;
	DWORD		TimeDateStamp;
	DWORD		PointerToSymbolTable;
	DWORD		NumberOfSymbols;
	WORD		SizeOfOptionalHeader;
	WORD		Characteristics;

	inline bool Is64() const
	{
		return Machine == IMAGE_FILE_MACHINE_AMD64;
	}

	inline bool Is32() const
	{
		return Machine == IMAGE_FILE_MACHINE_I386;
	}

	inline bool IsNotSupported() const
	{
		return !Is32() && !Is64();
	}
};

struct PE_DATA_DIRECTORY
{
	DWORD   VirtualAddress;
	DWORD   Size;
};

// PE_OPTIONAL_HEADER_64 = PE_NT_HEADER_64->OptionalHeader
struct PE_OPTIONAL_HEADER_64
{
	WORD		Magic;
	BYTE		MajorLinkerVersion;
	BYTE		MinorLinkerVersion;
	DWORD		SizeOfCode;
	DWORD		SizeOfInitializedData;
	DWORD		SizeOfUninitializedData;
	DWORD		AddressOfEntryPoint;
	DWORD		BaseOfCode;
	ULONGLONG	ImageBase;
	DWORD		SectionAlignment;
	DWORD		FileAlignment;
	WORD		MajorOperatingSystemVersion;
	WORD		MinorOperatingSystemVersion;
	WORD		MajorImageVersion;
	WORD		MinorImageVersion;
	WORD		MajorSubsystemVersion;
	WORD		MinorSubsystemVersion;
	DWORD		Win32VersionValue;
	DWORD		SizeOfImage;
	DWORD		SizeOfHeaders;
	DWORD		CheckSum;
	WORD		Subsystem;
	WORD		DllCharacteristics;
	ULONGLONG	SizeOfStackReserve;
	ULONGLONG	SizeOfStackCommit;
	ULONGLONG	SizeOfHeapReserve;
	ULONGLONG	SizeOfHeapCommit;
	DWORD		LoaderFlags;
	DWORD		NumberOfRvaAndSizes;
	PE_DATA_DIRECTORY	DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
};

// PE_OPTIONAL_HEADER_32 = PE_NT_HEADER_32->OptionalHeader
struct PE_OPTIONAL_HEADER_32
{
	//
	// Standard fields.
	//

	WORD		Magic;
	BYTE    	MajorLinkerVersion;
	BYTE    	MinorLinkerVersion;
	DWORD   	SizeOfCode;
	DWORD   	SizeOfInitializedData;
	DWORD   	SizeOfUninitializedData;
	DWORD   	AddressOfEntryPoint;
	DWORD   	BaseOfCode;
	DWORD   	BaseOfData;

	//
	// NT additional fields.
	//

	DWORD   	ImageBase;
	DWORD   	SectionAlignment;
	DWORD   	FileAlignment;
	WORD    	MajorOperatingSystemVersion;
	WORD    	MinorOperatingSystemVersion;
	WORD    	MajorImageVersion;
	WORD    	MinorImageVersion;
	WORD    	MajorSubsystemVersion;
	WORD    	MinorSubsystemVersion;
	DWORD   	Win32VersionValue;
	DWORD   	SizeOfImage;
	DWORD   	SizeOfHeaders;
	DWORD   	CheckSum;
	WORD    	Subsystem;
	WORD    	DllCharacteristics;
	DWORD   	SizeOfStackReserve;
	DWORD   	SizeOfStackCommit;
	DWORD   	SizeOfHeapReserve;
	DWORD   	SizeOfHeapCommit;
	DWORD   	LoaderFlags;
	DWORD   	NumberOfRvaAndSizes;
	PE_DATA_DIRECTORY	DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
};


// Parse unknown .EXE to check if it's x86 or x64
// https://learn.microsoft.com/vi-vn/windows/win32/api/winnt/ns-winnt-image_optional_header32
struct PE_NT_HEADER_MAGIC
{
	DWORD				Signature;
	PE_FILE_HEADER		FileHeader;
	WORD				Magic;

	// IMAGE_NT_OPTIONAL_HDR32_MAGIC (0x10B)		x86 Image
	// IMAGE_NT_OPTIONAL_HDR64_MAGIC (0x20B)		x64 Image
	// IMAGE_ROM_OPTIONAL_HDR_MAGIC  (0x107)		"The file is a ROM image."? I hope i'll never have to deal with this

	inline bool Is64() const
	{
		return Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC;
	}

	inline bool Is32() const
	{
		return Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC;
	}

	inline bool IsNotSupported() const
	{
		return !Is32() && !Is64();
	}
};

struct PE_NT_HEADER_64
{
	DWORD					Signature;
	PE_FILE_HEADER			FileHeader;
	PE_OPTIONAL_HEADER_64	OptionalHeader;

	inline PE_SECTION_HEADER* GetFirstSectionHeader()
	{
		return reinterpret_cast<PE_SECTION_HEADER*>((ULONG_PTR)&OptionalHeader + FileHeader.SizeOfOptionalHeader);
	}
};

struct PE_NT_HEADER_32
{
	DWORD					Signature;
	PE_FILE_HEADER			FileHeader;
	PE_OPTIONAL_HEADER_32	OptionalHeader;

	inline PE_SECTION_HEADER* GetFirstSectionHeader()
	{
		return reinterpret_cast<PE_SECTION_HEADER*>((ULONG_PTR)&OptionalHeader + FileHeader.SizeOfOptionalHeader);
	}
};

// PE_DOS_HEADER* = base;
struct PE_DOS_HEADER
{      
	WORD	e_magic;                     // Magic number
	WORD	e_cblp;                      // Bytes on last page of file
	WORD	e_cp;                        // Pages in file
	WORD	e_crlc;                      // Relocations
	WORD	e_cparhdr;                   // Size of header in paragraphs
	WORD	e_minalloc;                  // Minimum extra paragraphs needed
	WORD	e_maxalloc;                  // Maximum extra paragraphs needed
	WORD	e_ss;                        // Initial (relative) SS value
	WORD	e_sp;                        // Initial SP value
	WORD	e_csum;                      // Checksum
	WORD	e_ip;                        // Initial IP value
	WORD	e_cs;                        // Initial (relative) CS value
	WORD	e_lfarlc;                    // File address of relocation table
	WORD	e_ovno;                      // Overlay number
	WORD	e_res[4];                    // Reserved words
	WORD	e_oemid;                     // OEM identifier (for e_oeminfo)
	WORD	e_oeminfo;                   // OEM information; e_oemid specific
	WORD	e_res2[10];                  // Reserved words
	LONG	e_lfanew;                    // File address of new exe header
};


// PE_EXPORT_DIRECTORY* = _RVA_ PE_OPTIONAL_HEADER->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
struct PE_EXPORT_DIRECTORY
{
	DWORD   Characteristics;
	DWORD   TimeDateStamp;
	WORD    MajorVersion;
	WORD    MinorVersion;
	DWORD   Name;
	DWORD   Base;
	DWORD   NumberOfFunctions;
	DWORD   NumberOfNames;
	DWORD   AddressOfFunctions;			// RVA from base of image
	DWORD   AddressOfNames;				// RVA from base of image
	DWORD   AddressOfNameOrdinals;		// RVA from base of image
};


struct PE_ExportData
{
	UINT ordinal;		// ordinal index
	POINTER offset;		// offset from the base
	std::string name;	// function name
};

struct PE_ImportData
{

};

struct PE_DirectoryData
{
	std::vector<PE_ImportData> imports;
	std::vector<PE_ExportData> exports;
};

struct PE_Data
{
	IMAGE_DOS_HEADER DOS;
	PE_DirectoryData dirs;
};

struct ModuleData
{
	POINTER baseAddress = 0;
	std::string moduleNameA;
	std::string modulePathA;
	std::wstring moduleNameW;
	std::wstring modulePathW;
	PE_Data pe;
};
