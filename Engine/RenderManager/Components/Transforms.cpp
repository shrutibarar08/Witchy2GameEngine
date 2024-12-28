#include "Transforms.h"


Transforms::Transforms(const Drawable& parent)
	: m_parent(parent)
{
	if (m_vertexConstantBuffer == nullptr)
	{
		m_vertexConstantBuffer = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>();
	}
}

void Transforms::Bind() noexcept
{
	m_vertexConstantBuffer->Update(
		DirectX::XMMatrixTranspose(m_parent.GetTransformXM() * W2RenderAPI::Get()->GetProjection()));
	m_vertexConstantBuffer->Bind();
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> Transforms::m_vertexConstantBuffer;
