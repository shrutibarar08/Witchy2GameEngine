#include "ShapeSphere.h"
#include "RenderManager/RenderInterface.h"
#include "Primitives/PrimitiveSphere.h"


ShapeSphere::ShapeSphere(float radius)
{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
		};

		auto model = PrimitiveSphere::Make<Vertex>();
		model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
		AddBind(std::make_unique<VertexBuffer>(model.m_vertices));
		AddIndexBuffer(std::make_unique<IndexBuffer>(model.m_indices));

		auto pvs = std::make_unique<VertexShader>(L"Shaders/Compiled/SimpleVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));
		AddStaticBind(std::make_unique<PixelShader>(L"Shaders/Compiled/SimplePS.cso"));

		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSColorConstant>>(colorConst));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,
				0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(ied, pvsbc));
		AddStaticBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<Transforms>(*this));
}

void ShapeSphere::Update(float deltaTime) noexcept
{
}

void ShapeSphere::SetTranslation(DirectX::XMFLOAT3 translationVec) noexcept
{
	this->m_translation = translationVec;
}

DirectX::XMMATRIX ShapeSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z);
}
