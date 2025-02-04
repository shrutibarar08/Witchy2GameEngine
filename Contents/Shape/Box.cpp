#include "Box.h"
#include "RenderManager/RenderInterface.h"
#include "ExceptionManager/RenderAPIMacros.h"

#include "Primitives/PrimitiveSphere.h"
#include "Primitives/PrimitiveCone.h"
#include "Primitives/PrimitiveCube.h"


Box::Box(std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist)
	:
	r(rdist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dphi(odist(rng)),
	dtheta(odist(rng)),
	dchi(odist(rng)),
	chi(adist(rng)),
	theta(adist(rng)),
	phi(adist(rng))
{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT2 tex;
			DirectX::XMFLOAT3 norm;
		};
		auto model = PrimitiveCube()
		.BuildTexcord<Vertex>()
		.BuildPosition<Vertex>()
		.BuildNorm<Vertex>()
		.GetTopology<Vertex>();
		model.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.2f));
		// Update Color on Each vertex

		// TODO: Make Texture Coordinate Dynamic (Update Primitve Class to Consider Texture Coordinates as well).

		AddStaticBind(std::make_unique<VertexBuffer>(model.m_vertices));

		auto pvs = std::make_unique<VertexShader>(L"Shaders/Compiled/PointVS.cso");
		auto pvsbc = pvs->GetBytecode();

		AddStaticBind(std::move(pvs));
		AddStaticBind(std::make_unique<PixelShader>(L"Shaders/Compiled/PointPS.cso"));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(model.m_indices));

		//~ Set Pixel Constant Buffer
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
				0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{"TEXTCORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				20, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		AddStaticBind(std::make_unique<InputLayout>(ied, pvsbc));
		AddStaticBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else 
	{
		SetIndexFromStatic();
	}

	auto texture = std::make_unique<SurfaceTexture>();
	texture->AddTexture(L"Assets/Textures/CubTextures/pexels_didsss.dds", 0u);
	texture->AddSampler(); // TODO: Can be included inside add Texture since I only need it once.

	AddTexture(std::move(texture));

	struct PSMaterical
	{
		alignas(16) DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[2];	// ensuring 32 bytes
	} color = { {0.7f, 0.1f, 0.5f} };
	AddBind(std::make_unique<PixelConstantBuffer<PSMaterical>>(color, 1u));
	AddBind(std::make_unique<Transforms>(*this));

	DirectX::XMStoreFloat3x3(&mt, DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));
}

void Box::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3(&mt) *
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
