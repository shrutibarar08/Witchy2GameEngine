#pragma once
#include "RenderManager/W2RenderAPI.h"
#include "Shape/ShapeSphere.h"
#include "RenderManager/Components/ConstantBuffers.h"


class W2PointLight
{
public:
	struct LightBuffer
	{
		alignas(16) DirectX::XMFLOAT3 LightPosition;
		alignas(16) DirectX::XMFLOAT3 Ambient;
		alignas(16) DirectX::XMFLOAT3 DiffuseColor;
		float DiffuseIntensity;
		float AttConst;
		float AttLin;
		float AttQuad;
	};
public:
	W2PointLight(float radius = 1.f);
	void InitControlWindow() noexcept;
	void Draw(float deltaTime) noexcept;
	void Bind() const noexcept;
private:
	LightBuffer m_data
	{
		DirectX::XMFLOAT3(-70, 32, 42),
		DirectX::XMFLOAT3(0.45, 0.84, 0.80),
		DirectX::XMFLOAT3(1, 1, 1),
		1.0f,
		3.527,
		0.597,
		.0
	};
	mutable ShapeSphere m_sphereMesh;
	mutable PixelConstantBuffer<LightBuffer> m_lightBuffer;
};
