#pragma once
#include "stdafx.h"
#include "Win32Symbol.h"

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

	POINTER GetRVAOffset(const BYTE* pBase, DWORD virtualAddress)
	{
		PE_SECTION_HEADER* pSectionHeaders = GetFirstSectionHeader();
		// loop all sections to get the RVA offset
		for (size_t i = 0; i < FileHeader.NumberOfSections; i++)
		{
			auto sectionBegin = pSectionHeaders[i].VirtualAddress;
			auto sectionEnd = pSectionHeaders[i].VirtualAddress + pSectionHeaders[i].Misc.VirtualSize;

			// the directory virtual address is in this section, we have the RVA offset
			if (sectionBegin <= virtualAddress && virtualAddress <= sectionEnd)
			{
				PE_SECTION_HEADER& pSection = pSectionHeaders[i];
				POINTER RVA_offset = reinterpret_cast<POINTER>(pBase + pSection.PointerToRawData - pSection.VirtualAddress);

				return RVA_offset;
			}
		}

		return 0ull;
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

	POINTER GetRVAOffset(const BYTE* pBase, DWORD virtualAddress)
	{
		PE_SECTION_HEADER* pSectionHeaders = GetFirstSectionHeader();
		// loop all sections to get the RVA offset
		for (size_t i = 0; i < FileHeader.NumberOfSections; i++)
		{
			auto sectionBegin = pSectionHeaders[i].VirtualAddress;
			auto sectionEnd = pSectionHeaders[i].VirtualAddress + pSectionHeaders[i].Misc.VirtualSize;

			// the directory virtual address is in this section, we have the RVA offset
			if (sectionBegin <= virtualAddress && virtualAddress <= sectionEnd)
			{
				PE_SECTION_HEADER& pSection = pSectionHeaders[i];
				POINTER RVA_offset = reinterpret_cast<POINTER>(pBase + pSection.PointerToRawData - pSection.VirtualAddress);

				return RVA_offset;
			}
		}

		return 0ull;
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



namespace Engine
{
template <typename NtHeaderType>
_NODISCARD auto ParseNtHeader(void* pBase, ProcessModule& procMod)->SafeResult(void)
{

	auto* pByteBase = reinterpret_cast<BYTE*>(pBase);
	auto* pDosHeader = reinterpret_cast<PE_DOS_HEADER*>(pBase);
	auto* pNtHeader = reinterpret_cast<NtHeaderType*>(pByteBase + pDosHeader->e_lfanew);
	auto* pSectionHeaders = pNtHeader->GetFirstSectionHeader();

	// virtual address of the directory
	DWORD dirVirtualAddr = pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	POINTER RVA_offset = pNtHeader->GetRVAOffset(pByteBase, dirVirtualAddr);

	RESULT_FAILIFN(RVA_offset, SymbolEngineFailedToFindRelativeVirtualAddress);

	auto* pDirectory = reinterpret_cast<PE_EXPORT_DIRECTORY*>(RVA_offset + dirVirtualAddr);
	auto* pExportDirectory = reinterpret_cast<PE_EXPORT_DIRECTORY*>(RVA_offset + dirVirtualAddr);

	auto* lpFunctionNames = reinterpret_cast<DWORD*>(RVA_offset + pExportDirectory->AddressOfNames);
	auto* lpFunctions = reinterpret_cast<DWORD*>(RVA_offset + pExportDirectory->AddressOfFunctions);
	auto* lpOrdinals = reinterpret_cast<WORD*>(RVA_offset + pExportDirectory->AddressOfNameOrdinals);

	for (size_t j = 0; j < pExportDirectory->NumberOfNames; j++)
	{
		// ordinal of the export
		WORD ordinal = lpOrdinals[j];

		// offset of the export
		DWORD function = lpFunctions[ordinal];

		// symbol name
		char* name = reinterpret_cast<char*>(lpFunctionNames[j] + RVA_offset);

		ModuleSymbol& data = procMod.AddExportSymbol();
		data.offset = static_cast<POINTER>(function);
		data.ordinal = ordinal;
		data.name = name;
	}

	return {};
}

_NODISCARD auto GetPEInfo(ProcessModule& procMod)->SafeResult(void)
{
	// open the file
	HANDLE hSrcFile = CreateFileW(
		procMod.modulePathW.c_str(),
		GENERIC_READ, FILE_SHARE_READ,
		nullptr, OPEN_EXISTING, 0, nullptr
	);

	WINAPI_FAILIFN(hSrcFile != INVALID_HANDLE_VALUE, CannotParseImagePEHeader);

	// map the file into memory
	HANDLE hMapSrcFile = CreateFileMappingW(hSrcFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
	WINAPI_FAILIFN(hMapSrcFile != nullptr, CannotParseImagePEHeader);

	void* pVoidBase = MapViewOfFile(hMapSrcFile, FILE_MAP_READ, 0, 0, 0);
	WINAPI_FAILIFN(pVoidBase != nullptr, CannotParseImagePEHeader);

	BYTE* pBase = static_cast<BYTE*>(pVoidBase);
	auto* pDosHeader = static_cast<PE_DOS_HEADER*>(pVoidBase);

	// check if this file is x86 or x64
	auto pNtHeaderMagic = reinterpret_cast<PE_NT_HEADER_MAGIC*>(pBase + pDosHeader->e_lfanew);

	if (pNtHeaderMagic->IsNotSupported())
	{
		UnmapViewOfFile(pVoidBase);
		RESULT_THROW(TheImageFileFormatIsNotSupported);
	}

	// for 32 bit process
	if (pNtHeaderMagic->Is32())
	{
		// strip 4 bytes of the address if the module is 32-bit
		procMod.base &= 0xFFFFFFFFull;

		auto parseResult = ParseNtHeader<PE_NT_HEADER_32>(pVoidBase, procMod);
		//if (parseResult.has_error()) parseResult.error().show();
	}
	// for 64 bit process
	else
	{
		auto parseResult = ParseNtHeader<PE_NT_HEADER_64>(pVoidBase, procMod);
		//if (parseResult.has_error()) parseResult.error().show();
	}

	UnmapViewOfFile(pVoidBase);

	return {};
}

_NODISCARD auto Win32Symbol::Tlhelp32RetrieveModuleList()->SafeResult(void)
{
	//DWORD snapFlags = TH32CS_SNAPMODULE;
	//if (m_target->is32bit) snapFlags |= TH32CS_SNAPMODULE32;
	DWORD snapFlags = TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32;


	HANDLE hSnap = CreateToolhelp32Snapshot(snapFlags, m_target->pid);
	WINAPI_FAILIFN_NM(hSnap != INVALID_HANDLE_VALUE);

	MODULEENTRY32W modEntry{ 0 };
	modEntry.dwSize = sizeof(MODULEENTRY32W);

	m_target->modules.clear();

	if (!Module32FirstW(hSnap, &modEntry))
	{
		CloseHandle(hSnap);
		WINAPI_THROW(NoMessage);
	}

	do
	{
		ProcessModule& procMod = m_target->modules.emplace_back();
		procMod.base = reinterpret_cast<POINTER>(modEntry.modBaseAddr);
		procMod.size = static_cast<size_t>(modEntry.modBaseSize);
		procMod.moduleNameW = modEntry.szModule;
		procMod.modulePathW = modEntry.szExePath;
		procMod.moduleNameA = common::BhString(procMod.moduleNameW);
		procMod.modulePathA = common::BhString(procMod.modulePathW);

	} while (Module32NextW(hSnap, &modEntry));

	CloseHandle(hSnap);
	return {};
}

// TODO: This function is too long, split it into multiple functions
_NODISCARD auto Win32Symbol::K32RetrieveModuleList()->SafeResult(void)
{
	// init 256 module, if it wasn't enough we will allocate more
	if (m_bufferModule.size() == 0) m_bufferModule.resize(256);


	DWORD cbSizeNeeded = 0;
	DWORD cbCurrentSize = static_cast<DWORD>(m_bufferModule.size() * sizeof(HMODULE));

	// enum all the modules
	BOOL result =
		K32EnumProcessModulesEx(
			m_target->handle,
			m_bufferModule.data(),
			cbCurrentSize,
			&cbSizeNeeded,
			LIST_MODULES_ALL
		);

	WINAPI_FAILIFN(result, EnumProcessModulesFailed);

	// the m_bufferModule size is not enough didn't have enough space, create more
	if (cbSizeNeeded > cbCurrentSize)
	{
		m_bufferModule.resize(static_cast<size_t>(cbSizeNeeded) / sizeof(HMODULE));
		result =
			K32EnumProcessModulesEx(
				m_target->handle,
				m_bufferModule.data(),
				cbCurrentSize,
				&cbSizeNeeded,
				LIST_MODULES_64BIT
			);

		WINAPI_FAILIFN(result, EnumProcessModulesFailed);
	}

	size_t moduleCount = static_cast<size_t>(cbSizeNeeded) / sizeof(HMODULE);

	// clear the modules list, just to make sure
	m_target->modules.clear();
	m_target->modules.reserve(moduleCount);

	if (m_bufferModulePath.size() == 0) m_bufferModulePath.resize(2048);

	DWORD cbBufferModule = static_cast<DWORD>(m_bufferModulePath.size() * sizeof(wchar_t));

	// iterate through each module
	for (auto& hModule : m_bufferModule)
	{
		// some module is nullptr, no idea why
		if (!hModule) continue;

		ProcessModule& modData = m_target->modules.emplace_back();
		modData.base = reinterpret_cast<POINTER>(hModule);

		// get the full file path of the module
		if (
			K32GetModuleFileNameExW(
				m_target->handle,
				hModule,
				m_bufferModulePath.data(),
				cbBufferModule
			)
		)
		{
			// This is a bit dirty but it's fine, it's not like we call this function all the time
			// TODO: Rework common::BhString, BhPathGetTrail
			size_t length = wcsnlen_s(m_bufferModulePath.c_str(), m_bufferModulePath.size());

			modData.modulePathW = std::wstring(m_bufferModulePath.c_str(), length);
			modData.modulePathA = common::BhString(modData.modulePathW);
			modData.moduleNameW = common::BhPathGetTrail(modData.modulePathW);
			modData.moduleNameA = common::BhString(modData.moduleNameW);

		}
		else
		{
			modData.moduleNameA = "<NULL>";
			modData.moduleNameW = L"<NULL>";
		}

		// get module size and entry point
		MODULEINFO moduleInfo{ 0 };
		if (
			K32GetModuleInformation(
				m_target->handle,
				hModule,
				&moduleInfo,
				sizeof(moduleInfo)
			)
		)
		{
			modData.size = moduleInfo.SizeOfImage;
			modData.entryPoint = reinterpret_cast<POINTER>(moduleInfo.EntryPoint);
		}

	}

	return {};
}

_NODISCARD auto Win32Symbol::Update() -> SafeResult(std::vector<ProcessModule>&)
{
	if (auto result = Tlhelp32RetrieveModuleList(); result.has_error())
	{
		return cpp::fail(common::err(result.error()));
	}

	for (auto& procMod : m_target->modules)
	{
		if (auto result = GetPEInfo(procMod); result.has_error())
		{
			result.error().show();
			continue;
		}
	}

	// the base module is always at the front
	m_target->baseModule = &m_target->modules.front();

	UpdateModules(m_target->modules);
	return m_target->modules;
}


// format and address into module.function+xxxx
_NODISCARD auto Win32Symbol::AddressToSymbol(POINTER address) -> SafeResult(std::string)
{
	auto walkContext = AddressSymbolWalkInit();
	auto resultGetModuleSymbol = AddressSymbolWalkNext(walkContext, address);

	if (!resultGetModuleSymbol.has_value()) RESULT_THROW(NoMessage);

	static char symbol[1024];

	if (!resultGetModuleSymbol.has_symbol())
	{
		auto procMod = resultGetModuleSymbol.Module();
		if (address == procMod->base) return procMod->moduleNameA;

		sprintf_s(symbol, "%s+%llX", procMod->moduleNameA.c_str(), address - procMod->base);
		return symbol;
	}
	else
	{
		auto pModule = resultGetModuleSymbol.Module();
		auto pSymbol = resultGetModuleSymbol.Symbol();


		POINTER offsetFromVA = address - (pModule->base + pSymbol->offset);

		if (offsetFromVA > 0)
		{
			sprintf_s(
				symbol, "%s.%s+%llX",
				pModule->moduleNameA.c_str(),
				pSymbol->name.c_str(),
				offsetFromVA
			);
		}
		else
		{
			sprintf_s(symbol, "%s.%s", pModule->moduleNameA.c_str(), pSymbol->name.c_str());
		}

		return symbol;
	}

	RESULT_THROW(NoMessage);
}

// return an adddress from a symbol such as module.function+0xxxx
_NODISCARD auto Win32Symbol::SymbolToAddress(const std::string& symbol) -> SafeResult(POINTER)
{
	// TODO-IMPLEMENT
	POINTER x;
	std::stringstream ss;
	ss << std::hex << symbol;
	ss >> x;

	return x;
}

}