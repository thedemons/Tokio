#pragma once


class BaseMemory
{
protected:
	DWORD m_pid = 0;
	HANDLE m_handle = 0;

public:
	virtual auto Attach(DWORD pid) -> SafeResult(void) = 0;
	virtual void Detach()
	{
		if (m_pid) m_pid = 0;
		if (m_handle) CloseHandle(m_handle);
	}

	virtual auto Read(POINTER src, void* dest, size_t size) -> SafeResult(void) = 0;
	virtual auto Write(POINTER dest, const void* src, size_t size) -> SafeResult(void) = 0;

	template <typename Type>
	auto Read(POINTER address) -> SafeResult(Type)
	{
		Type value;
		auto result = Read(address, &value, static_cast<size_t>(sizeof(Type)));

		return result.has_error() ? result.error() : value;
	}

	template <typename Type>
	auto Write(POINTER address, Type value) -> SafeResult(void)
	{
		return Write(address, &value, static_cast<size_t>(sizeof(Type)));
	}

	virtual _CONSTEXPR20 DWORD GetPID()
	{
		return m_pid;
	}

	virtual _CONSTEXPR20 HANDLE GetHandle()
	{
		return m_handle;
	}

	virtual auto GetModuleList()->SafeResult(std::vector<ModuleData>) = 0;

	// Resolve module symbols
	virtual auto LoadSymbols(std::vector<ModuleData>& moduleList)->SafeResult(void)
	{
		//for (auto& modData : moduleList)
		//{

		//	HMODULE lib = LoadLibraryExA(filepath, NULL, DONT_RESOLVE_DLL_REFERENCES);
		//	//assert(((PIMAGE_DOS_HEADER)lib)->e_magic == IMAGE_DOS_SIGNATURE);
		//	PIMAGE_NT_HEADERS header = (PIMAGE_NT_HEADERS)((BYTE*)lib + ((PIMAGE_DOS_HEADER)lib)->e_lfanew);
		//	//assert(header->Signature == IMAGE_NT_SIGNATURE);
		//	//assert(header->OptionalHeader.NumberOfRvaAndSizes > 0);
		//	PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)lib + header->
		//		OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		//	//assert(exports->AddressOfNames != 0);
		//	BYTE** names = (BYTE**)((int)lib + exports->AddressOfNames);
		//	for (int i = 0; i < exports->NumberOfNames; i++)
		//		printf("Export: %s\n", (BYTE*)lib + (int)names[i]);

		//}
		return {};
	}
};