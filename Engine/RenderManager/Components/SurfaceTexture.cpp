#include "SurfaceTexture.h"
#include "DDSTextureLoader.h"

#include "ExceptionManager/RenderAPIMacros.h"


void SurfaceTexture::AddTexture(const std::wstring& texturePath)
{
	INIT_INFO();
	ID3D11ShaderResourceView* rv;
	RENDER_API_THROW(
		DirectX::CreateDDSTextureFromFile(
			W2RenderAPI::Get()->GetDevice(),
			texturePath.c_str(),
		nullptr, &rv));
	m_shaderRV.emplace_back(std::move(rv));
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

void SurfaceTexture::Bind() noexcept
{
	W2RenderAPI::Get()->GetDeviceContext()->PSSetShaderResources(0,
		m_shaderRV.size(), m_shaderRV.data());

	W2RenderAPI::Get()->GetDeviceContext()->PSSetSamplers(
		0u, m_samplerState.size(), m_samplerState.data()
	);
}
