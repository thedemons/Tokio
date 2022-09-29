
namespace PEParser
{

template <typename NtHeaderType>
auto ParseNtHeader(void* pBase) -> SafeResult(PE_DirectoryData)
{
	PE_DirectoryData result;

	auto* pByteBase = reinterpret_cast<BYTE*>(pBase);
	auto* pDosHeader = reinterpret_cast<PE_DOS_HEADER*>(pBase);
	auto* pNtHeader = reinterpret_cast<NtHeaderType*>(pByteBase + pDosHeader->e_lfanew);
	auto* pSectionHeaders = pNtHeader->GetFirstSectionHeader();

	// virtual address of the directory
	DWORD dirVirtualAddr = pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	for (size_t i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++)
	{
		auto sectionBegin = pSectionHeaders[i].VirtualAddress;
		auto sectionEnd = pSectionHeaders[i].VirtualAddress + pSectionHeaders[i].Misc.VirtualSize;

		if (sectionBegin <= dirVirtualAddr && dirVirtualAddr <= sectionEnd)
		{
			PE_SECTION_HEADER& pSection = pSectionHeaders[i];
			POINTER RVA_offset = reinterpret_cast<POINTER>(pByteBase + pSection.PointerToRawData - pSection.VirtualAddress);

			auto* pDirectory = reinterpret_cast<PE_EXPORT_DIRECTORY*>(RVA_offset + dirVirtualAddr);
			auto* pExportDirectory = reinterpret_cast<PE_EXPORT_DIRECTORY*>(RVA_offset + dirVirtualAddr);

			auto* lpFunctionNames = reinterpret_cast<DWORD*>(RVA_offset + pExportDirectory->AddressOfNames);
			auto* lpFunctions = reinterpret_cast<DWORD*>(RVA_offset + pExportDirectory->AddressOfFunctions);
			auto* lpOrdinals = reinterpret_cast<WORD*>(RVA_offset + pExportDirectory->AddressOfNameOrdinals);


			for (size_t j = 0; j < pExportDirectory->NumberOfNames; j++)
			{
				// TODO: FIX THIS DIRTY SHIT
				auto index = lpOrdinals[j];
				DWORD function = lpFunctions[index];
				auto x = (char*)(lpFunctionNames[j] + RVA_offset);

				PE_ExportData data;
				data.offset = static_cast<POINTER>(function);
				data.ordinal = index;
				data.name = x;

				result.exports.push_back(data);
			}
			break;
		}
	}

	return result;
}

auto GetPEInfo(const std::wstring& filePath) -> SafeResult(PE_Data)
{
	// open the file
	HANDLE hSrcFile = CreateFileW(
		filePath.c_str(),
		GENERIC_READ, FILE_SHARE_READ,
		nullptr, OPEN_EXISTING, 0, nullptr
	);

	WINAPI_FAILIFN(hSrcFile, CannotParseImagePEHeader);

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

	PE_Data result;

	if (pNtHeaderMagic->Is32())
	{
		auto parseResult = ParseNtHeader<PE_NT_HEADER_32>(pVoidBase);
		if (parseResult.has_value()) result.dirs = parseResult.value();
	}
	else
	{
		auto parseResult = ParseNtHeader<PE_NT_HEADER_64>(pVoidBase);
		if (parseResult.has_value()) result.dirs = parseResult.value();
	}

	UnmapViewOfFile(pVoidBase);
	return result;
}

}