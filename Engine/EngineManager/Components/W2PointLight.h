#pragma once
#include "RenderManager/W2RenderAPI.h"
#include "Shape/ShapeSphere.h"
#include "RenderManager/Components/ConstantBuffers.h"


class W2PointLight
{
public:
	struct LightBuffer
	{
		DirectX::XMFLOAT3 translation;
		float offset;
	};
public:
	W2PointLight(float radius = 1.f);
	void InitControlWindow() noexcept;
	void Draw() const noexcept;
	void Bind() const noexcept;
private:
	DirectX::XMFLOAT3 m_translation{ 0.0f, 0.0f, 0.0f };
	mutable ShapeSphere m_sphereMesh;
	mutable PixelConstantBuffer<LightBuffer> m_lightBuffer{};
};
