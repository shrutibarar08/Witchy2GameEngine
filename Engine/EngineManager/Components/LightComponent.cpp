#include "LightComponent.h"
#include "RenderManager/W2RenderAPI.h"


template <typename T>
LightComponent<T>::LightComponent(UINT slot)
	: m_slot(slot)
{}

template <typename T>
LightComponent<T>::~LightComponent()
{
	if (m_shaderRV) m_shaderRV->Release();
}

template<typename T>
UINT LightComponent<T>::GetBindSlot() const noexcept
{
	return m_slot;
}

template <typename T>
void LightComponent<T>::Bind() noexcept
{
	if (_update_required || m_shaderRV == nullptr)
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer = nullptr;

		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		desc.ByteWidth = sizeof(T) * m_lights.size();
		desc.StructureByteStride = sizeof(T);
		D3D11_SUBRESOURCE_DATA initData = { m_lights.data(), 0, 0 };
		W2RenderAPI::Get()->GetDevice()->CreateBuffer(&desc, &initData, &buffer);

		D3D11_SHADER_RESOURCE_VIEW_DESC vd{};
		vd.Format = DXGI_FORMAT_UNKNOWN;
		vd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		vd.Buffer.ElementOffset = 0u;
		vd.Buffer.NumElements = m_lights.size();
		W2RenderAPI::Get()->GetDevice()->CreateShaderResourceView(buffer.Get(), &vd, &m_shaderRV);
		_update_required = false;	// avoid recreating everytime
	}

	W2RenderAPI::Get()->SetPSShaderResources(m_shaderRV.Get(), m_slot);
}

template<typename T>
void LightComponent<T>::AddLight(T& lightSource) noexcept
{
	m_lights.emplace_back(std::move(lightSource));
	_update_required = true;
}
