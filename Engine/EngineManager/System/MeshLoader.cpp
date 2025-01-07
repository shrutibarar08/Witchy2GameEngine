#include "MeshLoader.h"
#include "ImuGui/imgui.h"
#include <unordered_map>
#include <sstream>

namespace dx = DirectX;


ModelException::ModelException(int line, const char* file, std::string note) noexcept
	:
	W2Exception(line, file),
	note(std::move(note))
{
}

const char* ModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << W2Exception::what() << std::endl
		<< "[Note] " << GetNote();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "MeshModel Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return note;
}

// Mesh
Mesh::Mesh(std::vector<std::unique_ptr<Bindable>> bindPtrs)
{
	if (!IsStaticInitialized())
	{
		AddStaticBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	for (auto& pb : bindPtrs)
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
void Mesh::Draw(DirectX::FXMMATRIX accumulatedTransform) const
{
	DirectX::XMStoreFloat4x4(&m_transform, accumulatedTransform);
	Drawable::Draw();
}
DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&m_transform);
}


// MeshNode
MeshNode::MeshNode(const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in)
	:
meshPtrs(std::move(meshPtrs)),
m_name(name)
{
	dx::XMStoreFloat4x4(&m_transform, transform_in);
	dx::XMStoreFloat4x4(&m_appliedTransform, dx::XMMatrixIdentity());
}

void MeshNode::Draw(DirectX::FXMMATRIX accumulatedTransform) const
{
	const auto built =
		dx::XMLoadFloat4x4(&m_appliedTransform) *
		dx::XMLoadFloat4x4(&m_transform) *
		accumulatedTransform;
	for (const auto pm : meshPtrs)
	{
		pm->Draw(built);
	}
	for (const auto& pc : ppChildNodes)
	{
		pc->Draw(built);
	}
}

void MeshNode::AddChild(std::unique_ptr<MeshNode> pChild)
{
	assert(pChild);
	ppChildNodes.push_back(std::move(pChild));
}

void MeshNode::DrawChild(int& nodeIndexTracked, std::optional<int>& selectedIndex, MeshNode*& pSelectedNode) const noexcept
{
	// nodeIndex serves as the uid for gui tree nodes, incremented throughout recursion
	const int currentNodeIndex = nodeIndexTracked;
	nodeIndexTracked++;
	// build up flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((currentNodeIndex == selectedIndex.value_or(-1)) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((ppChildNodes.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);
	// render this node
	const auto expanded = ImGui::TreeNodeEx(
		(void*)(intptr_t)currentNodeIndex, node_flags, m_name.c_str()
	);
	// processing for selecting node
	if (ImGui::IsItemClicked())
	{
		selectedIndex = currentNodeIndex;
		pSelectedNode = const_cast<MeshNode*>(this);
	}
	// recursive rendering of open node's children
	if (expanded)
	{
		for (const auto& pChild : ppChildNodes)
		{
			pChild->DrawChild(nodeIndexTracked, selectedIndex, pSelectedNode);
		}
		ImGui::TreePop();
	}
}

void MeshNode::SetTransform(DirectX::FXMMATRIX transform) noexcept
{
	dx::XMStoreFloat4x4(&m_appliedTransform, transform);
}


// MeshModel
class ModelWindow // pImpl idiom, only defined in this .cpp
{
public:
	void Show(const char* windowName, const MeshNode& root) noexcept
	{
		// window m_name defaults to "MeshModel"
		windowName = windowName ? windowName : "MeshModel";
		// need an ints to track node indices and selected node
		int nodeIndexTracker = 0;
		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);
			root.DrawChild(nodeIndexTracker, selectedIndex, pSelectedNode);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				auto& transform = transforms[*selectedIndex];
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
			}
		}
		ImGui::End();
	}
	dx::XMMATRIX GetTransform() const noexcept
	{
		const auto& transform = transforms.at(*selectedIndex);
		return
			dx::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
			dx::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}
	MeshNode* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}
private:
	std::optional<int> selectedIndex;
	MeshNode* pSelectedNode;
	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> transforms;
};

MeshModel::MeshModel(const std::string fileName)
	:
	pWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals
	);

	if (pScene == nullptr)
	{
		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
	}

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		ppMeshes.push_back(ParseMesh(*pScene->mMeshes[i]));
	}

	pRoot = ParseNode(*pScene->mRootNode);
}

void MeshModel::Draw() const
{
	if (auto node = pWindow->GetSelectedNode())
	{
		node->SetTransform(pWindow->GetTransform());
	}
	pRoot->Draw(dx::XMMatrixIdentity());
}

void MeshModel::ShowWindow(const char* windowName) noexcept
{
	pWindow->Show(windowName, *pRoot);
}

MeshModel::~MeshModel() noexcept
{
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
std::unique_ptr<MeshNode> MeshModel::ParseNode(const aiNode& node) noexcept
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
		curMeshPtrs.push_back(ppMeshes.at(meshIdx).get());
	}

	auto pNode = std::make_unique<MeshNode>(node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(*node.mChildren[i]));
	}

	return pNode;
}
