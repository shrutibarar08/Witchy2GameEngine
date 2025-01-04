#pragma once

#include "ConstantBuffers.h"
#include "RenderManager/Object/Drawable.h"
#include <DirectXMath.h>


class Transforms : public Bindable
{
	struct TRANSFORM_DESC
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX Projection;
	};
public:
	Transforms(const Drawable& parent, UINT slot = 0u);
	void Bind() noexcept override;
private:
	static std::unique_ptr<VertexConstantBuffer<TRANSFORM_DESC>> m_vertexConstantBuffer;
	const Drawable& m_parent;
};
