#include "TestObject.h"
#include "RenderManager/RenderInterface.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

#include "RenderManager/Core/Vertex.h"

TestObject::TestObject(std::mt19937& rng,
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
		using RenderCore::VertexLayout;
		RenderCore::VertexBuffer vertBuffer(std::move(VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
		));

		Assimp::Importer importer;
		const auto pModel = importer.ReadFile("Assets/Models/octopus_toy.obj", aiProcess_JoinIdenticalVertices | aiProcess_Triangulate);
		const auto pMesh = pModel->mMeshes[0];

		for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
		{
			vertBuffer.EmplaceBack(
				DirectX::XMFLOAT3{ pMesh->mVertices[i].x,pMesh->mVertices[i].y,pMesh->mVertices[i].z },
				*reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(pMesh->mNumFaces * 3);
		for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
		{
			const auto& face = pMesh->mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		AddStaticBind(std::make_unique<VertexBuffer>(vertBuffer));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(indices));

		auto pvs = std::make_unique<VertexShader>(L"Shaders/Compiled/PointVST.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(L"Shaders/Compiled/PointPST.cso"));
		AddStaticBind(std::make_unique<InputLayout>(vertBuffer.GetLayout().GetD3DLayout(), pvsbc));
		AddStaticBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterial
		{
			alignas(16) DirectX::XMFLOAT3 color;
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[2];	// ensuring 32 bytes
		} color = { {0.7f, 0.1f, 0.5f} };

		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterial>>(color, 1u));

	}else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<Transforms>(*this));
	DirectX::XMStoreFloat3x3(&mt, DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f));
}

void TestObject::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX TestObject::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3(&mt) *
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
