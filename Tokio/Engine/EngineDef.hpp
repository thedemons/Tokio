#pragma once

typedef unsigned __int64 POINTER;

// PE Header structs ===================================================

// PE_SECTION_HEADER* pFirstSectionHeader = 
// &PE_NT_HEADER->OptionalHeader + PE_NT_HEADER->FileHeader.SizeOfOptionalHeader
struct PE_SECTION_HEADER
{
	BYTE		Name[IMAGE_SIZEOF_SHORT_NAME]{0};
	union
	{
		DWORD   PhysicalAddress;
		DWORD   VirtualSize;
	} Misc                                   {0};
	DWORD		VirtualAddress               {0};
	DWORD		SizeOfRawData                {0};
	DWORD		PointerToRawData             {0};
	DWORD		PointerToRelocations         {0};
	DWORD		PointerToLinenumbers         {0};
	WORD		NumberOfRelocations          {0};
	WORD		NumberOfLinenumbers          {0};
	DWORD		Characteristics              {0};
};

// PE_FILE_HEADER = PE_NT_HEADER->FileHeader
struct PE_FILE_HEADER
{
	WORD		Machine{0};
	WORD		NumberOfSections{0};
	DWORD		TimeDateStamp{0};
	DWORD		PointerToSymbolTable{0};
	DWORD		NumberOfSymbols{0};
	WORD		SizeOfOptionalHeader{0};
	WORD		Characteristics{0};

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
	DWORD   VirtualAddress{0};
	DWORD   Size{0};
};

// PE_OPTIONAL_HEADER_64 = PE_NT_HEADER_64->OptionalHeader
struct PE_OPTIONAL_HEADER_64
{
	WORD		Magic                      {0};
	BYTE		MajorLinkerVersion         {0};
	BYTE		MinorLinkerVersion         {0};
	DWORD		SizeOfCode                 {0};
	DWORD		SizeOfInitializedData      {0};
	DWORD		SizeOfUninitializedData    {0};
	DWORD		AddressOfEntryPoint        {0};
	DWORD		BaseOfCode                 {0};
	ULONGLONG	ImageBase                  {0};
	DWORD		SectionAlignment           {0};
	DWORD		FileAlignment              {0};
	WORD		MajorOperatingSystemVersion{0};
	WORD		MinorOperatingSystemVersion{0};
	WORD		MajorImageVersion          {0};
	WORD		MinorImageVersion          {0};
	WORD		MajorSubsystemVersion      {0};
	WORD		MinorSubsystemVersion      {0};
	DWORD		Win32VersionValue          {0};
	DWORD		SizeOfImage                {0};
	DWORD		SizeOfHeaders              {0};
	DWORD		CheckSum                   {0};
	WORD		Subsystem                  {0};
	WORD		DllCharacteristics         {0};
	ULONGLONG	SizeOfStackReserve         {0};
	ULONGLONG	SizeOfStackCommit          {0};
	ULONGLONG	SizeOfHeapReserve          {0};
	ULONGLONG	SizeOfHeapCommit           {0};
	DWORD		LoaderFlags                {0};
	DWORD		NumberOfRvaAndSizes        {0};
	PE_DATA_DIRECTORY	DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
};

// PE_OPTIONAL_HEADER_32 = PE_NT_HEADER_32->OptionalHeader
struct PE_OPTIONAL_HEADER_32
{
	//
	// Standard fields.
	//

	WORD		Magic                      {0};
	BYTE    	MajorLinkerVersion         {0};
	BYTE    	MinorLinkerVersion         {0};
	DWORD   	SizeOfCode                 {0};
	DWORD   	SizeOfInitializedData      {0};
	DWORD   	SizeOfUninitializedData    {0};
	DWORD   	AddressOfEntryPoint        {0};
	DWORD   	BaseOfCode                 {0};
	DWORD   	BaseOfData                 {0};

	//
	// NT additional fields.
	//

	DWORD   	ImageBase                  {0};
	DWORD   	SectionAlignment           {0};
	DWORD   	FileAlignment              {0};
	WORD    	MajorOperatingSystemVersion{0};
	WORD    	MinorOperatingSystemVersion{0};
	WORD    	MajorImageVersion          {0};
	WORD    	MinorImageVersion          {0};
	WORD    	MajorSubsystemVersion      {0};
	WORD    	MinorSubsystemVersion      {0};
	DWORD   	Win32VersionValue          {0};
	DWORD   	SizeOfImage                {0};
	DWORD   	SizeOfHeaders              {0};
	DWORD   	CheckSum                   {0};
	WORD    	Subsystem                  {0};
	WORD    	DllCharacteristics         {0};
	DWORD   	SizeOfStackReserve         {0};
	DWORD   	SizeOfStackCommit          {0};
	DWORD   	SizeOfHeapReserve          {0};
	DWORD   	SizeOfHeapCommit           {0};
	DWORD   	LoaderFlags                {0};
	DWORD   	NumberOfRvaAndSizes        {0};
	PE_DATA_DIRECTORY	DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
};


// Parse unknown .EXE to check if it's x86 or x64
// https://learn.microsoft.com/vi-vn/windows/win32/api/winnt/ns-winnt-image_optional_header32
struct PE_NT_HEADER_MAGIC
{
	DWORD				Signature{0};
	PE_FILE_HEADER		FileHeader{0};
	WORD				Magic{0};

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
	DWORD					Signature{0};
	PE_FILE_HEADER			FileHeader{0};
	PE_OPTIONAL_HEADER_64	OptionalHeader{0};

	inline PE_SECTION_HEADER* GetFirstSectionHeader()
	{
		return reinterpret_cast<PE_SECTION_HEADER*>((ULONG_PTR)&OptionalHeader + FileHeader.SizeOfOptionalHeader);
	}
};

struct PE_NT_HEADER_32
{
	DWORD					Signature{0};
	PE_FILE_HEADER			FileHeader{0};
	PE_OPTIONAL_HEADER_32	OptionalHeader{0};

	inline PE_SECTION_HEADER* GetFirstSectionHeader()
	{
		return reinterpret_cast<PE_SECTION_HEADER*>((ULONG_PTR)&OptionalHeader + FileHeader.SizeOfOptionalHeader);
	}
};

// PE_DOS_HEADER* = base;
struct PE_DOS_HEADER
{      
	WORD	e_magic   {0};                        // Magic number
	WORD	e_cblp    {0};                        // Bytes on last page of file
	WORD	e_cp      {0};                        // Pages in file
	WORD	e_crlc    {0};                        // Relocations
	WORD	e_cparhdr {0};                        // Size of header in paragraphs
	WORD	e_minalloc{0};                        // Minimum extra paragraphs needed
	WORD	e_maxalloc{0};                        // Maximum extra paragraphs needed
	WORD	e_ss      {0};                        // Initial (relative) SS value
	WORD	e_sp      {0};                        // Initial SP value
	WORD	e_csum    {0};                        // Checksum
	WORD	e_ip      {0};                        // Initial IP value
	WORD	e_cs      {0};                        // Initial (relative) CS value
	WORD	e_lfarlc  {0};                        // File address of relocation table
	WORD	e_ovno    {0};                        // Overlay number
	WORD	e_res[4]  {0};                        // Reserved words
	WORD	e_oemid   {0};                        // OEM identifier (for e_oeminfo)
	WORD	e_oeminfo {0};                        // OEM information; e_oemid specific
	WORD	e_res2[10]{0};                        // Reserved words
	LONG	e_lfanew  {0};                        // File address of new exe header
};


// PE_EXPORT_DIRECTORY* = _RVA_ PE_OPTIONAL_HEADER->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
struct PE_EXPORT_DIRECTORY
{
	DWORD   Characteristics      {0};
	DWORD   TimeDateStamp        {0};
	WORD    MajorVersion         {0};
	WORD    MinorVersion         {0};
	DWORD   Name                 {0};
	DWORD   Base                 {0};
	DWORD   NumberOfFunctions    {0};
	DWORD   NumberOfNames        {0};
	DWORD   AddressOfFunctions   {0};			    // RVA from base of image
	DWORD   AddressOfNames       {0};				// RVA from base of image
	DWORD   AddressOfNameOrdinals{0};		        // RVA from base of image
};


struct PE_ExportData
{
	UINT ordinal  {0};		// ordinal index
	POINTER offset{0};		// offset from the base
	std::string name;	    // function name
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
	PE_DOS_HEADER DOS{0};
	PE_DirectoryData dirs;
};

// Module information, including PE header info
struct ModuleData
{
	POINTER address = 0;
	std::string moduleNameA;
	std::string modulePathA;
	std::wstring moduleNameW;
	std::wstring modulePathW;
	PE_Data pe;
};

// Contains all information about the target process
struct ProcessData
{
	// process id
	DWORD pid{0}; 

	// process handle for memory operation
	HANDLE handle{0};

	// all loaded modules
	std::vector<ModuleData> modules;

	// is the process x86 or x64
	BOOL is32bit = false;

	// the base image module
	inline ModuleData& base() { return modules[0]; }
};


// The type of address, is it belong to a module, a function, or unknown?
enum class AddressType
{
	Unknown,
	Module,
	Function,
};

struct DisasmData
{
	POINTER address = 0;
	AddressType addressType = AddressType::Unknown;
	std::string addressFormat;
	std::string instruction;
};
