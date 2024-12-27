#pragma once

#include "ConstantBuffers.h"
#include "Renderer/Object/Drawable.h"
#include <DirectXMath.h>


class Transforms : public Bindable
{
public:
	Transforms(const Drawable& parent);
	void Bind() noexcept override;
private:
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> m_vertexConstantBuffer;
	const Drawable& m_parent;
};
