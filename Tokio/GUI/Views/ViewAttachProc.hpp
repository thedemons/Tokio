#pragma once


namespace ViewAttachProcUtils
{

std::unordered_map<HICON, std::wstring> m_iconIndices;
std::unordered_map<std::wstring, ID3D11ShaderResourceView*> m_icons;

auto GetTextureFromBitmap(void* buffer, UINT width, UINT height) -> SafeResult(ID3D11ShaderResourceView*)
{
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = buffer;
	subResource.SysMemPitch = width * 4;
	subResource.SysMemSlicePitch = 0;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = 0;

	ID3D11Texture2D* pTexture = nullptr;
	HRESULT hr = MainWindow::g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);
	DX_FAILIFN(hr, NoMessage);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* pResource = nullptr;
	hr = MainWindow::g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &pResource);

	if (hr != S_OK)
	{
		pTexture->Release();
		DX_FAILIFN(hr, NoMessage);
	}

	return pResource;
}

auto GetFileHIcon(const std::wstring& path) -> SafeResult(HICON)
{
	DWORD attrs = 0;
	std::error_code ec;
	UINT flags = SHGFI_ICON | SHGFI_LARGEICON;

	if (!std::filesystem::exists(path, ec))
	{
		flags |= SHGFI_USEFILEATTRIBUTES;
		attrs = FILE_ATTRIBUTE_DIRECTORY;
	}

	SHFILEINFOW fileInfo = { 0 };
	SHGetFileInfoW(path.c_str(), attrs, &fileInfo, sizeof(SHFILEINFOW), flags);

	RESULT_FAILIFN_NM(fileInfo.hIcon != nullptr);
	return fileInfo.hIcon;
}

auto GetFileIconTexture(const std::wstring& path) -> SafeResult(ID3D11ShaderResourceView*)
{
	// Thanks to the author of the code
	// https://github.com/dfranx/ImFileDialog/blob/main/ImFileDialog.h

	// return the stored texture if we have loaded it already
	if (m_icons.count(path) > 0)
		return m_icons[path];


	auto resultGHI = GetFileHIcon(path);
	RESULT_FAILIFN_NM(resultGHI.has_value());

	HICON hFileIcon = resultGHI.value();

	// check if hFileIcon is already loaded
	// but the path is of another file
	auto findIcon = m_iconIndices.find(hFileIcon);
	if (findIcon != m_iconIndices.end())
	{
		auto& icon = m_icons[findIcon->second];
		m_icons[path] = icon;
		return icon;
	}

	// Get the icon info 
	ICONINFO iconInfo = { 0 };
	WINAPI_FAILIFN_NM(GetIconInfo(hFileIcon, &iconInfo));

	// get bitmap info
	DIBSECTION ds;
	WINAPI_FAILIFN_NM(GetObjectW(iconInfo.hbmColor, sizeof(ds), &ds));

	UINT width = ds.dsBm.bmWidth;
	UINT height = ds.dsBm.bmHeight;

	// calculate the buffer size needed
	// ds.dsBm.bmBitsPixel is the number of bit per pixel
	int byteSize = width * height * (ds.dsBm.bmBitsPixel / 8); 

	// copy bitmap to the buffer
	std::vector<BYTE> bitmapBuffer(byteSize);
	LONG resultGetBm = GetBitmapBits(iconInfo.hbmColor, byteSize, bitmapBuffer.data());
	RESULT_FAILIFN_NM(resultGetBm);


	// create the texture
	auto resultGTFI = GetTextureFromBitmap(bitmapBuffer.data(), width, height);
	RESULT_FAILIFN_NM(resultGTFI.has_value());

	// cache the icon
	m_iconIndices[hFileIcon] = path;
	m_icons[path] = resultGTFI.value();

	return resultGTFI.value();
}
}

class ViewAttachProc : public BaseView
{
private:
	struct ProcessData
	{
		DWORD pid = 0;			// process pid
		HWND hwnd = 0;			// an associated window
		std::string name;		// process name
		std::wstring wname;		// wide process name
		std::wstring wpath;		// full file path
		std::string winTitle;	// window title
		std::string winClass;	// window class name
		uint64_t creationTime;	// FILETIME
		PROCESSENTRY32W entry;	// just in case
		ID3D11ShaderResourceView* icon = nullptr; // icon texture
	};
	

	Widgets::Table m_table;
	Widgets::TextInput m_textFilter;

	bool m_isOpenned = false;
	std::vector<ProcessData> m_processList;

	static Widgets::Table::Execution
	TableRenderCallback(Widgets::Table* table, UINT row, void* UserData)
	{
		ViewAttachProc* pThis = static_cast<ViewAttachProc*>(UserData);
		ProcessData& procData = pThis->m_processList[row];

		if (pThis->IsItemFiltered(procData)) return Widgets::Table::Execution::Skip;

		// icon
		if (procData.icon) ImGui::Image(procData.icon, { 15, 15 });

		// process
		ImGui::TableNextColumn();
		ImGui::Text("%s", procData.name.c_str());

		// window
		ImGui::TableNextColumn();
		if (procData.winTitle.size() > 0)
		{
			ImGui::Text("%s", procData.winTitle.c_str());
			ImGui::SameLine();
			ImGui::TextDisabled("// %s", procData.winClass.c_str());
		}

		// pid
		ImGui::TableNextColumn();
		ImGui::Text("%-6d | 0x%x", procData.pid, procData.pid);

		return Widgets::Table::Execution::Continue;
	};

	static void TableSortCallback(Widgets::Table* table, UINT column, ImGuiSortDirection direction, void* UserData)
	{
		ViewAttachProc* pThis = static_cast<ViewAttachProc*>(UserData);
		bool isAscending = direction == ImGuiSortDirection_Ascending;

		// using multiple different function instead of
		// using switch(case) in one function helps a lot with the performance
		// not really crucial here but i don't like slow things..

		// sort by the process' creation time
		auto sortByTime = [=](const ProcessData& a, const ProcessData& b) -> bool
		{
			return isAscending ?
				a.creationTime < b.creationTime : 
				a.creationTime > b.creationTime;
		};

		// sort by process' name
		auto sortByProcess = [=](const ProcessData& a, const ProcessData& b) -> bool
		{
			int cmp = std::strcmp(a.name.c_str(), b.name.c_str());
			if (cmp == 0) return sortByTime(a, b);

			return isAscending ? cmp < 0 : cmp > 0;
		};
		
		// sort by process' window title
		auto sortByWindow = [=](const ProcessData& a, const ProcessData& b) -> bool
		{
			int cmp = std::strcmp(a.winTitle.c_str(), b.winTitle.c_str());
			if (cmp == 0) return sortByTime(a, b);

			return isAscending ? cmp < 0 : cmp > 0;
		};

		// sort by process' pid
		auto sortByPID = [=](const ProcessData& a, const ProcessData& b) -> bool
		{
			return isAscending ? a.pid < b.pid : a.pid > b.pid;
		};
		
		// i know i know, but making a switch statement here would
		// make the code super duper long and i don't think i'd like that
		std::function sortFunction = sortByTime;
		if		(column == 1) sortFunction = sortByProcess;
		else if (column == 2) sortFunction = sortByWindow;
		else if (column == 3) sortFunction = sortByPID;

		std::sort(pThis->m_processList.begin(), pThis->m_processList.end(), sortFunction);
	}

public:
	ViewAttachProc()
	{
		Widgets::Table::Desc desc;
		desc.Name = "##TableAttachProc";
		desc.RenderCallback = TableRenderCallback;
		desc.SortCallback = TableSortCallback;
		desc.Flags = ImGuiTableFlags_RowBg							| // two background mode
					 ImGuiTableFlags_Borders						| // row borders
					 ImGuiTableFlags_ScrollY						| // enable vertical scrool
					 ImGuiTableFlags_Hideable						| // hide the columns
					 ImGuiTableFlags_Sortable						| // enable sorting
					 ImGuiTableFlags_Resizable						| // not sure what this does
					 ImGuiTableFlags_Reorderable					; // re-order the coulmns

		m_table.Setup(desc);

		// flags for the first column, aka the icon/creation time
		auto colFlags = ImGuiTableColumnFlags_NoHide				| // don't hide me please
						ImGuiTableColumnFlags_NoResize				| // do not resize me also
						ImGuiTableColumnFlags_WidthFixed			| // need this to initialize the width
						ImGuiTableColumnFlags_DefaultSort			| // default sort by creation time
						ImGuiTableColumnFlags_PreferSortDescending	; // latest spawned process will be on top

		m_table.AddColumn("# ", 15.f, colFlags);
		m_table.AddColumn("Process");
		m_table.AddColumn("Window");
		m_table.AddColumn("PID");

		Widgets::TextInput::Desc tiDesc;
		tiDesc.Label = "##Filter";
		tiDesc.Hint = "Filter";
		m_textFilter.Setup(tiDesc);

	}

	inline std::string Title() override
	{
		return "Attach Process";
	}

	inline bool isClosable() override
	{
		return false;
	}

	inline bool defaultOpenMode() override
	{
		return true;
	}

	void Render(bool& bOpen) override
	{
		if (m_isOpenned != bOpen)
		{
			m_isOpenned = bOpen;
			if (m_isOpenned)
			{
				GetProcessList();
				ImGui::OpenPopup("Attach Proccess");
			}
		}

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 0,0 });
		if (ImGui::BeginPopupModal("Attach Proccess", &m_isOpenned))
		{
			ImGui::PopStyleVar();

			ImGui::SetNextItemWidth(-1);

			m_textFilter.Render();
			m_table.Render(static_cast<UINT>(m_processList.size()), {0.f, 0.f}, this);
			ImGui::EndPopup();
		}
		else
		{
			bOpen = false;
			ImGui::PopStyleVar();
		}

	}

private:

	bool IsItemFiltered(const ProcessData& data)
	{
		if (m_textFilter.length() == 0 || m_textFilter.data()[0] == 0) return false;

		std::string filter = common::BhStringLower(m_textFilter.str_strip());

		if (common::BhStringLower(data.name).find(filter) != std::string::npos) return false;
		if (common::BhStringLower(data.winTitle).find(filter) != std::string::npos) return false;

		std::string pidInt = std::to_string(data.pid);
		if (pidInt.find(filter) != std::string::npos) return false;

		std::stringstream pidHexStream;
		pidHexStream << std::hex << data.pid;
		if (pidHexStream.str().find(filter) != std::string::npos) return false;

		return true;
	}

	void GetProcessList()
	{
		auto procList = common::BhGetAllProcess();
		if (procList.has_error())
		{
			procList.error().show(L"Cannot get process list");
			return;
		}

		// clear the current list first
		m_processList.clear();

		// push these processes into m_processList
		for (auto& [pid, entry] : procList.value())
		{
			ProcessData procData;
			procData.pid = pid;
			procData.entry = entry;
			procData.wname = entry.szExeFile;
			procData.name = common::BhString(procData.wname);
			procData.creationTime = 0;

			// get the full file path of process
			if (HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, pid))
			{
				// get full file path
				{
					DWORD sizeRequired = BHMAX_PATH;
					procData.wpath.resize(BHMAX_PATH);

					QueryFullProcessImageNameW(hProc, 0, procData.wpath.data(), &sizeRequired);

					procData.wpath.resize(sizeRequired);

					// get the file icon
					if (sizeRequired > 0)
					{
						auto fileIcon = ViewAttachProcUtils::GetFileIconTexture(procData.wpath);
						if (fileIcon.has_value()) procData.icon = fileIcon.value();
					}
				}

				// get creation time
				{
					FILETIME creationTime, exitTime, kernelTime, userTime;
					if (GetProcessTimes(hProc, &creationTime, &exitTime, &kernelTime, &userTime))
					{
						procData.creationTime = *(uint64_t*)&creationTime;
					}
				}

			}
			m_processList.push_back(procData);
		}

		// find their associated hwnd
		auto windowList = common::BhGetAllWindows();
		if (windowList.has_error())
		{
			windowList.error().show(L"Cannot get window list");
		}
		else
		{
			auto& winList = windowList.value();
			for (auto& procData : m_processList)
			{
				for (auto& winData : winList)
				{
					if (winData.pid != procData.pid) continue;

					procData.hwnd = winData.hwnd;
					procData.winTitle = common::BhString(winData.title);
					procData.winClass = common::BhString(winData.classname);
					break;
				}
			}
		}

		// manually trigger the sort because we
		// have reset the process list
		// REMEMBER TO PASS THE USERDATA!!!
		m_table.Sort(this);
	}
};