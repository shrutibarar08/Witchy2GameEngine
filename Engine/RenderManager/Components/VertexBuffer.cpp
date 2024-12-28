#include "VertexBuffer.h"

void VertexBuffer::Bind() noexcept
{
	const UINT offset = 0u;
	W2RenderAPI::Get()->GetDeviceContext()->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(),
		&m_stride, &offset);
}
