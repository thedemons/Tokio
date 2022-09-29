#pragma once


namespace ViewAttachProcUtils
{

std::unordered_map<HICON, ID3D11ShaderResourceView*> m_iconIndices;
std::unordered_map<HWND, ID3D11ShaderResourceView*> m_windowIcons;
std::unordered_map<std::wstring, ID3D11ShaderResourceView*> m_fileIcons;

// Thanks to the author of this awesome library, it helps me alot
// https://github.com/dfranx/ImFileDialog/blob/main/ImFileDialog.h

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
	HRESULT hr = MainApplication::g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);
	DX_FAILIFN(hr, NoMessage);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* pResource = nullptr;
	hr = MainApplication::g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &pResource);

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
	UINT flags = SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES;

	if (!std::filesystem::exists(path, ec))
	{
		attrs = FILE_ATTRIBUTE_DIRECTORY;
	}

	SHFILEINFOW fileInfo = { 0 };
	SHGetFileInfoW(path.c_str(), attrs, &fileInfo, sizeof(SHFILEINFOW), flags);

	RESULT_FAILIFN_NM(fileInfo.hIcon != nullptr);
	return fileInfo.hIcon;
}

auto GetTextureFromHIcon(const HICON hIcon) -> SafeResult(ID3D11ShaderResourceView*)
{
	// check if hFileIcon is already loaded
	auto findIcon = m_iconIndices.find(hIcon);
	if (findIcon != m_iconIndices.end()) return findIcon->second;

	// Get the icon info 
	ICONINFO iconInfo = { 0 };
	WINAPI_FAILIFN_NM(GetIconInfo(hIcon, &iconInfo));

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
	m_iconIndices[hIcon] = resultGTFI.value();

	return resultGTFI.value();
}

auto GetFileIconTexture(const std::wstring& path) -> SafeResult(ID3D11ShaderResourceView*)
{
	// return the stored texture if we have loaded it already
	auto findIcon = m_fileIcons.find(path);
	if (findIcon != m_fileIcons.end()) return findIcon->second;

	auto resultGHI = GetFileHIcon(path);
	RESULT_FAILIFN_NM(resultGHI.has_value());

	HICON hFileIcon = resultGHI.value();

	auto resultGTFHI = GetTextureFromHIcon(hFileIcon);
	RESULT_FAILIFN_NM(resultGTFHI.has_value());

	auto texture = resultGTFHI.value();
	m_fileIcons[path] = texture;

	return texture;
}

auto GetWindowIconTexture(const HWND hwnd) -> SafeResult(ID3D11ShaderResourceView*)
{
	// return the stored texture if we have loaded it already
	auto findIcon = m_windowIcons.find(hwnd);
	if (findIcon != m_windowIcons.end()) return findIcon->second;

	HICON hIcon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, -14));

	auto resultGTFHI = GetTextureFromHIcon(hIcon);
	RESULT_FAILIFN_NM(resultGTFHI.has_value());

	auto texture = resultGTFHI.value();
	m_windowIcons[hwnd] = texture;

	return texture;
}


}

class ViewAttachProc : public BaseView
{
private: // Table Callback
	static Widgets::Table::Execution
	TableRenderCallback(Widgets::Table* table, UINT index, void* UserData)
	{
		ViewAttachProc* pThis = static_cast<ViewAttachProc*>(UserData);
		ProcessData& procData = pThis->m_processList[index];

		if (pThis->IsItemFiltered(procData)) return Widgets::Table::Execution::Skip;

		// icon
		if (procData.icon) ImGui::Image(procData.icon, { 15, 15 });

		// process
		ImGui::TableNextColumn();
		ImGui::Text("%s", procData.name.c_str());

		// window
		ImGui::TableNextColumn();
		if (procData.windows.size() > 0)
		{
			auto& firstWindow = procData.windows[0];
			if (procData.windows.size() == 1)
			{
				ImGui::TreeNodeEx(firstWindow.hidden_title.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);

				ImGui::SameLine();
				if (firstWindow.icon)
				{
					ImGui::Image(firstWindow.icon, { 15, 15 });
					ImGui::SameLine();
				}
				ImGui::Text("%s", firstWindow.title.c_str());
			}
			else
			{
				// rendering image in tree node
				// https://github.com/ocornut/imgui/issues/282
				bool bTreeOpen = ImGui::TreeNodeEx(firstWindow.hidden_title.c_str());

				ImGui::SameLine();

				if (firstWindow.icon)
				{
					ImGui::Image(firstWindow.icon, { 15, 15 });
					ImGui::SameLine();
				}

				ImGui::Text("%s", firstWindow.title.c_str());
				if (bTreeOpen)
				{
					UINT skip_first = 0;
					for (auto& window : procData.windows)
					{
						if (skip_first++ == 0) continue;

						if (window.icon)
						{
							ImGui::Image(window.icon, { 15, 15 });
							ImGui::SameLine();
						}

						ImGui::Text("%s", window.title.c_str());
					}
					ImGui::TreePop();
				}
			}

		}

		// pid
		ImGui::TableNextColumn();
		ImGui::Text("%-6d | 0x%x", procData.pid, procData.pid);

		// file location
		// pid
		ImGui::TableNextColumn();
		ImGui::Text("%s", procData.path.c_str());
		return Widgets::Table::Execution::Continue;
	};

	static void TableSortCallback(Widgets::Table* table, UINT column, ImGuiSortDirection direction, void* UserData)
	{
		ViewAttachProc* pThis = static_cast<ViewAttachProc*>(UserData);
		bool isAscending = direction == ImGuiSortDirection_Ascending;

		DWORD oldSelectedPID = 0;
		if (auto& oldSelectedItems = pThis->m_table.GetSelectedItems(); oldSelectedItems.size() > 0)
		{
			oldSelectedPID = pThis->m_processList[oldSelectedItems[0]].pid;
		}

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

		// TODO: Implement per-window table
		// sort by process' window title
		auto sortByWindow = [=](const ProcessData& a, const ProcessData& b) -> bool
		{
			bool aHasWindow = a.windows.size() > 0;
			bool bHasWindow = b.windows.size() > 0;

			// if both don't have window(s)
			if (!aHasWindow && !bHasWindow) return sortByTime(a, b);

			if (!aHasWindow && bHasWindow) return isAscending;
			if (aHasWindow && !bHasWindow) return !isAscending;

			// both have window(s)

			int cmp = std::strcmp(a.windows[0].title.c_str(), b.windows[0].title.c_str());
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

		// restore the selected process
		if (oldSelectedPID != 0) pThis->SelectProcessByPid(oldSelectedPID);
	}

	static void TableInputCallback(Widgets::Table* table, INT index, void* UserData)
	{
		ViewAttachProc* pThis = static_cast<ViewAttachProc*>(UserData);

		// if any row is hovered
		if (index >= 0)
		{
			if (ImGui::IsMouseReleased(1))
				table->m_popup.Open();

			if (ImGui::IsMouseDoubleClicked(0))
			{
				DWORD pid = pThis->m_processList[index].pid;
				auto resultAttach = Engine::Attach(pid);
				if (resultAttach.has_error())
				{
					resultAttach.error().show();
				}
				else
				{
					ImGui::CloseCurrentPopup();
				}
			}
		}
	}

	static void TablePopupRenderCallback(Widgets::Table* table, UINT index, void* UserData)
	{
		index = table->GetSelectedItems()[0];

		ViewAttachProc* pThis = static_cast<ViewAttachProc*>(UserData);
		ProcessData& procData = pThis->m_processList[index];
	
		if (procData.icon != nullptr)
		{
			ImGui::Image(procData.icon, { 15, 15 });
			ImGui::SameLine();
		}

		ImGui::Text("%s", procData.name.c_str());
		ImGui::Separator();

		if (ImGui::Selectable("Open file location"))
		{
			common::BhOpenFileInExplorer(procData.wpath);
		}

		if (ImGui::Selectable("Kill process"))
		{

		}

		if (ImGui::Selectable("Suspend process"))
		{

		}
	}

private: // members
	struct WindowData
	{
		HWND hwnd = 0;								// window handle
		DWORD pid = 0;								// associated process
		std::string title;							// window title
		std::string classname;						// window class name
		std::string unique_title; 					// for tree node rendering
		std::string hidden_title; 					// for tree node rendering
		ID3D11ShaderResourceView* icon = nullptr;	// window icon texture
	};

	struct ProcessData
	{
		DWORD pid = 0;								// process pid
		std::string  name;							// process name
		std::string  path;							// file path
		std::wstring wname;							// wide process name
		std::wstring wpath;							// wide file path
		uint64_t creationTime = 0;					// creation time
		PROCESSENTRY32W entry{};					// just in case
		
		std::vector<WindowData> windows;			// associated windows
		ID3D11ShaderResourceView* icon = nullptr;	// process icon texture
	};

	Widgets::Table m_table;							// main table
	Widgets::TextInput m_textFilter;				// text input for filtering processes

	bool m_isOpenned = false;						// is the popup currenly openned
	std::vector<ProcessData> m_processList;			// list of all process

	double m_timeLastRefresh = 0.f;					// last refresh time, for refreshing the process list every x ms
	double m_refreshInterval = 1.f;					// refresh every 1000ms

public:
	ViewAttachProc()
	{
		m_title = u8"🖥 Attach Process";

		Widgets::Table::Desc desc;
		desc.Name = "##TableAttachProc";

		desc.SortUserData = this;
		desc.InputUserData = this;
		desc.RenderUserData = this;
		desc.PopupRenderUserData = this;

		desc.SortCallback = TableSortCallback;
		desc.InputCallback = TableInputCallback;
		desc.RenderCallback = TableRenderCallback;
		desc.PopupRenderCallback = TablePopupRenderCallback;

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

		m_table.AddColumn("", colFlags, 15.f);
		m_table.AddColumn("Process");
		m_table.AddColumn("Window");
		m_table.AddColumn("PID");
		m_table.AddColumn("File Location", ImGuiTableColumnFlags_DefaultHide);

		Widgets::TextInput::Desc tiDesc;
		tiDesc.Label = "##Filter";
		tiDesc.Hint = "Filter";
		m_textFilter.Setup(tiDesc);

	}


	_CONSTEXPR20 bool isClosable() const  override
	{
		return false;
	}

	_CONSTEXPR20 bool isDefaultOpen()  const override
	{
		return true;
	}

	_CONSTEXPR20 bool isAllowMultipleViews()  const override
	{
		return false;
	}

	void Render(bool& bOpen) override
	{
		if (m_isOpenned != bOpen)
		{
			m_isOpenned = bOpen;
			if (m_isOpenned)
			{
				GetProcessList();
				ImGui::OpenPopup(Title().c_str());
			}
		}

		if (!m_isOpenned) return;

		// check for refresh interval
		double timeSinceLastRefresh = ImGui::GetTime() - m_timeLastRefresh;
		if (timeSinceLastRefresh > m_refreshInterval)
			GetProcessList();

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, { 0,0 });

		if (ImGui::BeginPopupModal(Title().c_str(), &m_isOpenned))
		{
			ImGui::PopStyleVar();

			ImGui::SetNextItemWidth(-1);
			m_textFilter.Render();

			m_table.Render(static_cast<UINT>(m_processList.size()));

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
		// if the filter is empty then it's pass
		if (m_textFilter.length() == 0 || m_textFilter.data()[0] == 0) return false;

		std::string filter = common::BhStringLower(m_textFilter.str_strip());

		// return true if the text passed the filter
		const auto checkStr = [=](const std::string& text) -> bool
		{
			std::string textLower = common::BhStringLower(text);
			return textLower.find(filter) != std::string::npos;
		};

		// check the process name
		if (checkStr(data.name)) return false;

		// check the process' windows
		for (auto& window : data.windows)
		{
			if (checkStr(window.title)) return false;
			if (checkStr(window.classname)) return false;
		}


		// TODO: FIX THIS
		char parseString[64];
		sprintf_s(parseString, "%d 0x%x", data.pid, data.pid);
		if (checkStr(parseString)) return false;

		return true;
	}

	void SelectProcessByPid(DWORD pid)
	{
		for (auto& procData : m_processList)
		{
			if (procData.pid == pid)
			{
				UINT index = static_cast<UINT>(&procData - &m_processList[0]);
				m_table.AddSelectedItem(index);
				break;
			}
		}
	}

	void GetProcessList()
	{
		auto procList = common::BhGetAllProcess();
		if (procList.has_error())
		{
			procList.error().show(L"Cannot get process list");
			return;
		}
		
		// remember the current selected process in the table
		DWORD oldSelectedPID = 0;
		if (auto& oldSelectedItems = m_table.GetSelectedItems(); oldSelectedItems.size() > 0)
		{
			oldSelectedPID = m_processList[oldSelectedItems[0]].pid;
		}

		// clear the current list first
		m_processList.clear();
		m_table.ClearSelectedItems();

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
						procData.path = common::BhString(procData.wpath);

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
					if (winData.title.size() == 0) continue;

					// MIGHT WANNA DISABLE THIS!!
					// skip system's non-relavant windows
					if (winData.title == L"MSCTFIME UI" || winData.title == L"Default IME") continue;

					procData.windows.push_back(WindowData());
					WindowData& window = procData.windows.back();

					window.pid = procData.pid;
					window.hwnd = winData.hwnd;
					window.title = common::BhString(winData.title);
					window.classname = common::BhString(winData.classname);
					window.unique_title = GUIUtils::GetUniqueName(window.title, window.pid + window.hwnd);
					window.hidden_title = std::string("##hidden_") + window.unique_title;

					// get window icon
					auto windowIcon = ViewAttachProcUtils::GetWindowIconTexture(window.hwnd);
					if (windowIcon.has_value())
						window.icon = windowIcon.value();
					else
						window.icon = procData.icon;
				}

				if (procData.windows.size() > 1)
				{
					// sort the windows to display the longest 
					// title on top, prefer ones with an icon
					static const auto sortWindow = [](const WindowData& a, const WindowData& b) -> bool
					{
						if ((a.icon != nullptr && b.icon != nullptr) ||
							(a.icon == nullptr && b.icon == nullptr))
							return a.title.size() > b.title.size();

						return a.icon != nullptr;
					};

					std::sort(procData.windows.begin(), procData.windows.end(), sortWindow);
				}
			}
		}

		// manually trigger the sort because we
		// have reset the process list
		m_table.Sort();

		m_timeLastRefresh = ImGui::GetTime();

		// restore the selected process
		if (oldSelectedPID != 0) SelectProcessByPid(oldSelectedPID);
	}
};