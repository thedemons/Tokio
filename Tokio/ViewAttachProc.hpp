#pragma once

namespace ViewAttachProcUtils
{

std::unordered_map<HICON, std::wstring> m_iconIndices;
std::unordered_map<std::wstring, ID3D11ShaderResourceView*> m_icons;

auto GetTextureFromHIcon(void* buffer, UINT width, UINT height) -> SafeResult(ID3D11ShaderResourceView*)
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

	DIBSECTION ds;
	ICONINFO iconInfo = { 0 };
	GetIconInfo(hFileIcon, &iconInfo);

	RESULT_FAILIFN_NM(iconInfo.hbmColor != nullptr);
	RESULT_FAILIFN_NM(GetObjectW(iconInfo.hbmColor, sizeof(ds), &ds));

	int byteSize = ds.dsBm.bmWidth * ds.dsBm.bmHeight * (ds.dsBm.bmBitsPixel / 8);
	std::vector<BYTE> bitmapBuffer(byteSize);

	LONG resultGetBm = GetBitmapBits(iconInfo.hbmColor, byteSize, bitmapBuffer.data());
	RESULT_FAILIFN_NM(resultGetBm);

	auto resultGTFI = GetTextureFromHIcon(bitmapBuffer.data(), ds.dsBm.bmWidth, ds.dsBm.bmHeight);
	RESULT_FAILIFN_NM(resultGTFI.has_value());

	m_iconIndices[hFileIcon] = path;
	m_icons[path] = resultGTFI.value();

	return resultGTFI.value();
}
}

class ViewAttachProc : public BaseView
{
public:
	ID3D11ShaderResourceView* text;
	ViewAttachProc()
	{
		auto gfcResult = ViewAttachProcUtils::GetFileIconTexture(L"C:\\Program Files\\Process Hacker\\README.txt");
		if (gfcResult.has_value()) text = gfcResult.value();
	}

	inline std::string Title() override
	{
		return "Attach Process";
	}

	inline bool isClosable() override
	{
		return false;
	}

	void Render(bool* bOpen) override
	{
		ImGui::Begin(Title().c_str());
		ImGui::Image(text, { 30, 30 });
		ImGui::End();
	}
};