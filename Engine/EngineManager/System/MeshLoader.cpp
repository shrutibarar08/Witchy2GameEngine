#include "MeshLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

Mesh::Mesh(std::vector<std::unique_ptr<Bindable>> ppBinds)
{
	if (!IsStaticInitialized())
	{
		AddStaticBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	for (auto& pb : ppBinds)
	{
		if (auto pi = dynamic_cast<IndexBuffer*>(pb.get()))
		{
			AddIndexBuffer(std::unique_ptr<IndexBuffer>{ pi });
			pb.release();
		}
		else
		{
			AddBind(std::move(pb));
		}
	}

	AddBind(std::make_unique<Transforms>(*this));
}

void Mesh::Draw(DirectX::FXMMATRIX transformations) const noexcept
{
	DirectX::XMStoreFloat4x4(&m_transform, transformations);
	Drawable::Draw();
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&m_transform);
}

MeshNode::MeshNode(std::vector<Mesh*> ppMesh, const DirectX::XMMATRIX& transform) noexcept
	:
	m_ppMesh(std::move(ppMesh))
{
	DirectX::XMStoreFloat4x4(&this->m_transform, transform);
}

void MeshNode::Draw(DirectX::FXMMATRIX transformations) const noexcept
{
	const auto built = DirectX::XMLoadFloat4x4(&m_transform) * transformations;
	for (const auto pm : m_ppMesh)
	{
		pm->Draw(built);
	}
	for (const auto& pc : m_ppNode)
	{
		pc->Draw(built);
	}
}

void MeshNode::AddChild(std::unique_ptr<MeshNode> pChild) noexcept
{
	assert(pChild);
	m_ppNode.push_back(std::move(pChild));
}

MeshModel::MeshModel(const std::string fileName)
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices
	);

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		m_ppMesh.push_back(ParseMesh(*pScene->mMeshes[i]));
	}

	m_pRootNode = ParseNode(*pScene->mRootNode);
}

std::unique_ptr<Mesh> MeshModel::ParseMesh(const aiMesh& mesh)
{
	namespace dx = DirectX;
	using RenderCore::VertexLayout;

	RenderCore::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
	));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
		);
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	std::vector<std::unique_ptr<Bindable>> bindablePtrs;

	bindablePtrs.push_back(std::make_unique<VertexBuffer>(vbuf));

	bindablePtrs.push_back(std::make_unique<IndexBuffer>(indices));

	auto pvs = std::make_unique<VertexShader>(L"Shaders/Compiled/PointVST.cso");
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));

	bindablePtrs.push_back(std::make_unique<PixelShader>(L"Shaders/Compiled/PointPST.cso"));

	bindablePtrs.push_back(std::make_unique<InputLayout>(vbuf.GetLayout().GetD3DLayout(), pvsbc));

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} pmc;
	bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(pmc, 1u));

	return std::make_unique<Mesh>(std::move(bindablePtrs));
}

std::unique_ptr<MeshNode> MeshModel::ParseNode(const aiNode& node)
{
	namespace dx = DirectX;
	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(m_ppMesh.at(meshIdx).get());
	}

	auto pNode = std::make_unique<MeshNode>(std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(*node.mChildren[i]));
	}

	return pNode;
}

void MeshModel::Draw() const
{
	m_pRootNode->Draw(DirectX::XMMatrixIdentity());
}
