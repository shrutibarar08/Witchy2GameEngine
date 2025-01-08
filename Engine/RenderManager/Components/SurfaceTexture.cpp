#include "SurfaceTexture.h"
#include "DDSTextureLoader.h"

#include "ExceptionManager/RenderAPIMacros.h"


void SurfaceTexture::AddTexture(const std::wstring& texturePath, UINT slot)
{
	if (m_cacheInfo[texturePath]) return;

	INIT_INFO();
	ID3D11ShaderResourceView* rv;
	RENDER_API_THROW(
		DirectX::CreateDDSTextureFromFile(
			W2RenderAPI::Get()->GetDevice(),
			texturePath.c_str(),
		nullptr, &rv));
	m_shaderRV[slot].emplace_back(std::move(rv));
	m_cacheInfo[texturePath] = true;
	m_topTexture = texturePath;
}

void SurfaceTexture::AddTexture(const std::string& texturePath, UINT slot)
{
	AddTexture(std::string(m_topTexture.begin(), m_topTexture.end()), slot);
}

void SurfaceTexture::AddFileTexture(const std::string& texturePath, UINT slot)
{
	INIT_INFO();
	DirectX::ScratchImage image = GetScratchImage(texturePath);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture = nullptr;
	D3D11_TEXTURE2D_DESC texDesc{};
	texDesc.Width = static_cast<UINT>(image.GetMetadata().width);
	texDesc.Height = static_cast<UINT>(image.GetMetadata().height);
	texDesc.MipLevels = static_cast<UINT>(image.GetMetadata().mipLevels);
	texDesc.ArraySize = 1;
	texDesc.Format = m_format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = image.GetImages()->pixels;
	initData.SysMemPitch = static_cast<UINT>(image.GetImages()->rowPitch);
	initData.SysMemSlicePitch = static_cast<UINT>(image.GetImages()->slicePitch);

	RENDER_API_THROW(W2RenderAPI::Get()->GetDevice()->CreateTexture2D(&texDesc, &initData, &texture));

	ID3D11ShaderResourceView* srv = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

	RENDER_API_THROW(W2RenderAPI::Get()->GetDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, &srv));
	m_shaderRV[slot].emplace_back(std::move(srv));
}

DirectX::ScratchImage SurfaceTexture::GetScratchImage(const std::string& texturePath)
{
	INIT_INFO();
	DirectX::ScratchImage image;
	DirectX::TexMetadata metadata;

	std::wstring wPath = std::wstring(texturePath.begin(), texturePath.end());

	RENDER_API_THROW(DirectX::LoadFromWICFile(wPath.c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, nullptr, image));

	if (image.GetImage(0, 0, 0)->format != m_format)
	{
		DirectX::ScratchImage converted;
		RENDER_API_THROW(DirectX::Convert(*image.GetImage(0, 0, 0),
			m_format,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			converted));

		image = std::move(converted);
	}
	return std::move(image);
}

void SurfaceTexture::UpdateTexture(const std::wstring& texturePath, UINT slot)
{
	m_shaderRV.clear();
	m_cacheInfo.clear();
	AddTexture(texturePath, slot);
}

void SurfaceTexture::UpdateTexture(const std::string& texturePath, UINT slot)
{
	UpdateTexture(std::wstring(texturePath.begin(), texturePath.end()), slot);
}

std::string SurfaceTexture::GetTopTexture() const
{
	return std::string(m_topTexture.begin(), m_topTexture.end());
}

void SurfaceTexture::AddSampler()
{
	INIT_INFO();
	D3D11_SAMPLER_DESC sd{};
	ZeroMemory(&sd, sizeof(sd));

	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.MinLOD = 1;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* ss;

	RENDER_API_THROW
	(W2RenderAPI::Get()->GetDevice()->CreateSamplerState
		(&sd, &ss));

	m_samplerState.emplace_back(std::move(ss));
}

bool SurfaceTexture::Empty() const
{
	return m_shaderRV.empty();
}

SurfaceTexture::~SurfaceTexture()
{
	for (auto& resource: m_shaderRV)
	{
		for (auto& x : resource.second) x->Release();
	}
	for (auto& x: m_samplerState) x->Release();
}

void SurfaceTexture::Bind() noexcept
{
	for (auto& resource: m_shaderRV)
	{
		W2RenderAPI::Get()->SetPSShaderResources(resource.second, resource.first);
	}
	W2RenderAPI::Get()->GetDeviceContext()->PSSetSamplers(
		0u, m_samplerState.size(), m_samplerState.data()->GetAddressOf()
	);
}
