#pragma once

#include "Bindable.h"
#include "ExceptionManager/RenderAPIMacros.h"


template<typename T>
class ConstantBuffer : public Bindable
{
public:
	void Update(const T& data)
	{
		INIT_INFO();

		D3D11_MAPPED_SUBRESOURCE msr;
		RENDER_API_THROW(W2RenderAPI::Get()->GetDeviceContext()->Map(
			pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		));
		memcpy(msr.pData, &data, sizeof(data));
		W2RenderAPI::Get()->GetDeviceContext()->Unmap(pConstantBuffer.Get(), 0u);
	}

	ConstantBuffer(const T& data, UINT slot = 0u): m_slot(slot)
	{
		INIT_INFO();

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(data);
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &data;
		RENDER_API_THROW(W2RenderAPI::Get()->GetDevice()->CreateBuffer(&cbd, &csd, &pConstantBuffer));
	}

	ConstantBuffer(UINT slot = 0u): m_slot(slot)
	{
		INIT_INFO();

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(T);
		cbd.StructureByteStride = 0u;
		RENDER_API_THROW(W2RenderAPI::Get()->GetDevice()->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	UINT m_slot;
};

template<typename T>
class VertexConstantBuffer : public ConstantBuffer<T>
{
	using ConstantBuffer<T>::pConstantBuffer;
	using ConstantBuffer<T>::m_slot;
public:
	using ConstantBuffer<T>::ConstantBuffer;
	void Bind() noexcept override
	{
		W2RenderAPI::Get()->GetDeviceContext()->VSSetConstantBuffers(m_slot, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename T>
class PixelConstantBuffer : public ConstantBuffer<T>
{
	using ConstantBuffer<T>::pConstantBuffer;
	using ConstantBuffer<T>::m_slot;
public:
	using ConstantBuffer<T>::ConstantBuffer;
	void Bind() noexcept override
	{
		W2RenderAPI::Get()->GetDeviceContext()->PSSetConstantBuffers(m_slot, 1u, pConstantBuffer.GetAddressOf());
	}
};
