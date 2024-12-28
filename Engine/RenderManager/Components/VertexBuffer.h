#pragma once

#include "Bindable.h"
#include "ExceptionManager/RenderAPIMacros.h"


class VertexBuffer : public Bindable
{
public:
	template<typename T>
	VertexBuffer(const std::vector<T>& vertices)
		: m_stride(sizeof(T))
	{
		INIT_INFO();

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(T));
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0u;
		vertexBufferDesc.MiscFlags = 0u;
		vertexBufferDesc.StructureByteStride = sizeof(T);
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA vertexBufferData = {};
		vertexBufferData.pSysMem = vertices.data();

		RENDER_API_THROW(W2RenderAPI::Get()->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer));

	}
	void Bind() noexcept override;
private:
	UINT m_stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
};

