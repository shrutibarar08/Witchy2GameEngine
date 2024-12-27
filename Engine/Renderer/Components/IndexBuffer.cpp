#include "IndexBuffer.h"
#include "Exceptions/RenderAPIMacros.h"

IndexBuffer::IndexBuffer(const std::vector<unsigned short>& indices)
	:
	m_count((UINT)indices.size())
{
	INIT_INFO();

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = UINT(m_count * sizeof(unsigned short));
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();
	RENDER_API_THROW(W2RenderAPI::Get()->GetDevice()->CreateBuffer(&ibd, &isd, &pIndexBuffer));
}

void IndexBuffer::Bind() noexcept
{
	W2RenderAPI::Get()->GetDeviceContext()->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::GetCount() const noexcept
{
	return m_count;
}
