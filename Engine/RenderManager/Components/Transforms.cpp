#include "Transforms.h"
#include "EngineManager/Player/W2Camera.h"


Transforms::Transforms(const Drawable& parent, UINT slot)
	: m_parent(parent)
{
	if (m_vertexConstantBuffer == nullptr)
	{
		m_vertexConstantBuffer = std::make_unique<VertexConstantBuffer<TRANSFORM_DESC>>(slot);
	}
}

void Transforms::Bind() noexcept
{
	const auto transform = m_parent.GetTransformXM() * W2Camera::Get()->GetViewMatrix();
	const TRANSFORM_DESC td
	{
		DirectX::XMMatrixTranspose(transform),
		DirectX::XMMatrixTranspose(transform * W2Camera::Get()->GetProjectionMatrix()),
	};
	m_vertexConstantBuffer->Update(td);
	m_vertexConstantBuffer->Bind();
}

std::unique_ptr<VertexConstantBuffer<Transforms::TRANSFORM_DESC>> Transforms::m_vertexConstantBuffer;
