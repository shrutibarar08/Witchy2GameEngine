#include "Topology.h"

Topology::Topology(D3D11_PRIMITIVE_TOPOLOGY type)
	: m_type(type)
{}

void Topology::Bind() noexcept
{
	W2RenderAPI::Get()->GetDeviceContext()->IASetPrimitiveTopology(m_type);
}
