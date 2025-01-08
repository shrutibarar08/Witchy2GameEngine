#include "MeshLoader.h"
#include "ImuGui/imgui.h"
#include "RenderManager/Components/SurfaceTexture.h"
#include <unordered_map>
#include <sstream>

#include "EngineManager/Player/W2Camera.h"
#include "WindowsManager/W2WindowAPI.h"

#include <random>

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

	AddBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

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
MeshNode::MeshNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in)
	:
m_name(name),
meshPtrs(std::move(meshPtrs)),
m_id(id)
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

ModelWindow::ModelWindow(const std::string& name, const TRANSFORM_PARAM_DESC& param)
: m_name(name)
{
	transforms[0] = param;
}

void ModelWindow::Show(const char* windowName, const MeshNode& root, float deltaTime, bool controller) noexcept
{
	// window m_name defaults to "MeshModel"
	windowName = windowName ? windowName : m_name.c_str();
	// need an ints to track node indices and selected node
	int nodeIndexTracker = 0;
	if (!controller && ImGui::Begin(windowName))
	{
		ImGui::Columns(2, nullptr, true);
		root.DrawChild(pSelectedNode);
		ImGui::NextColumn();
		if (pSelectedNode != nullptr)
		{
			auto& transform = transforms[pSelectedNode->GetId()];
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
			ImGui::Text("Position");
			ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
			ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
			ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
		}
		ImGui::End();
	}

	if (W2WindowAPI::Get()->Keyboard.KeyIsPressed('F'))
	{
		std::cout << "I am: " << m_name << "\n";
		std::cout << "Translation: " << transforms[root.GetId()].x << ", "
			<< transforms[root.GetId()].y << ", " << transforms[root.GetId()].z << "\n";

		std::cout << "Rotation: " << transforms[root.GetId()].roll << ", "
			<< transforms[root.GetId()].pitch << ", " << transforms[root.GetId()].yaw << "\n";
	}

	static bool turn = false;
	if (W2WindowAPI::Get()->Keyboard.KeyIsPressed('D') && controller)
	{
		transforms[root.GetId()].x += deltaTime * 10;

		if (transforms[root.GetId()].roll >= 0.2) turn = true;
		if (transforms[root.GetId()].roll <= -0.2) turn = false;

		if (!turn) transforms[root.GetId()].roll += deltaTime;
		else transforms[root.GetId()].roll -= deltaTime;
	}

	if (W2WindowAPI::Get()->Keyboard.KeyIsPressed('A') && controller)
	{
		transforms[root.GetId()].x -= deltaTime * 10;

		if (transforms[root.GetId()].roll >= 0.2) turn = false;
		if (transforms[root.GetId()].roll <= -0.2) turn = true;

		if (!turn) transforms[root.GetId()].roll -= deltaTime;
		else transforms[root.GetId()].roll += deltaTime;
	}

	if (W2WindowAPI::Get()->Keyboard.KeyIsPressed('W') && controller)
	{
		transforms[root.GetId()].z -= deltaTime * 20;
	}

	if (W2WindowAPI::Get()->Keyboard.KeyIsPressed('S') && controller)
	{
		transforms[root.GetId()].z += deltaTime * 20;
	}
}

void ModelWindow::Launch(float deltaTime) noexcept
{
	if (!m_launched)
	{
		//~ Do Something
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dist(-15.0, 0.0);
		transforms.at(0).z = dist(gen);
		transforms.at(0).y = 0.0f;
		transforms.at(0).x = m_rightHand;
		m_launched = true;
	}

	if (m_launched)
	{
		transforms.at(0).x -= deltaTime * 10.f;
	}

	if (transforms.at(0).x <= m_leftHand) m_launched = false;
}

DirectX::XMMATRIX ModelWindow::GetTransform() const noexcept
{
	const auto& transform = transforms.at(0);
	return
		DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
		DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
}

MeshNode* ModelWindow::GetSelectedNode() const noexcept
{
	return pSelectedNode;
}

void MeshNode::DrawChild(MeshNode*& pSelectedNode) const noexcept
{
	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((ppChildNodes.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);
	const auto expanded = ImGui::TreeNodeEx(
		reinterpret_cast<void*>(static_cast<intptr_t>(GetId())), node_flags, m_name.c_str()
	);
	if (ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<MeshNode*>(this);
	}
	if (expanded)
	{
		for (const auto& pChild : ppChildNodes)
		{
			pChild->DrawChild(pSelectedNode);
		}
		ImGui::TreePop();
	}
}

void MeshNode::SetTransform(DirectX::FXMMATRIX transform) noexcept
{
	dx::XMStoreFloat4x4(&m_appliedTransform, transform);
}

int MeshNode::GetId() const noexcept
{
	return m_id;
}

MeshModel::MeshModel(const std::string fileName, const ModelWindow::TRANSFORM_PARAM_DESC& param, bool normalize)
	:
	pWindow(std::make_unique<ModelWindow>(fileName, param)), m_normalize(normalize)
{
	for (int i = fileName.size() - 1; i >= 0; i--)
	{
		if (fileName[i] == '\\' || fileName[i] == '/')
		{
			m_basePath = std::string(fileName.begin(), fileName.begin() + i);
			std::cout << "Selected Base: " << m_basePath << "\n";
			m_basePath += '/';
			break;
		}
	}

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
		ppMeshes.push_back(ParseMesh(*pScene->mMeshes[i], pScene->mMaterials));
	}

	int rootId = 0;
	pRoot = ParseNode(rootId, *pScene->mRootNode);
}

void MeshModel::Draw() const
{
	pRoot->SetTransform(pWindow->GetTransform());
	pRoot->Draw(dx::XMMatrixIdentity());
}

void MeshModel::ShowWindow(float deltaTime, const char* windowName, bool controller) noexcept
{
	pWindow->Show(windowName, *pRoot, deltaTime, controller);
}

void MeshModel::Launch(float deltaTime) noexcept
{
	pWindow->Launch(deltaTime);
}

MeshModel::~MeshModel() noexcept
{
}

std::unique_ptr<Mesh> MeshModel::ParseMesh(const aiMesh& mesh, const aiMaterial* const* pMaterial)
{
	using RenderCore::VertexLayout;

	RenderCore::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Texture2D)
		.Append(VertexLayout::Normal)
	));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		if (m_normalize)
		{
			vbuf.EmplaceBack(
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i].Normalize()),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}else
		{
			vbuf.EmplaceBack(
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}
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

	std::vector<std::unique_ptr<Bindable>> ppBinds;
	std::unique_ptr<SurfaceTexture> tex = std::make_unique<SurfaceTexture>();
	tex->AddSampler();

	if (mesh.mMaterialIndex >= 0)
	{
		using namespace std::string_literals;
		auto& material = *pMaterial[mesh.mMaterialIndex];

		aiString texturePath;
		material.GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);

		if (texturePath.length)
		{
			const auto x =  m_basePath + texturePath.C_Str();
			std::cout << x << "\n";
			tex->AddFileTexture(x, 0u);
		}

		material.GetTexture(aiTextureType_SPECULAR, 0, &texturePath);

		if (texturePath.length)
		{
			const auto x = m_basePath + texturePath.C_Str();
			std::cout << x << "\n";
			tex->AddFileTexture(x, 1u);
		}
	}

	if (!tex->Empty())
	{
		ppBinds.push_back(std::move(tex));
	}

	ppBinds.push_back(std::make_unique<VertexBuffer>(vbuf));
	ppBinds.push_back(std::make_unique<IndexBuffer>(indices));

	auto pvs = std::make_unique<VertexShader>(L"Shaders/Compiled/PointVS.cso");
	auto pvsbc = pvs->GetBytecode();

	ppBinds.push_back(std::move(pvs));
	ppBinds.push_back(std::make_unique<PixelShader>(L"Shaders/Compiled/PointPS.cso"));
	ppBinds.push_back(std::make_unique<InputLayout>(vbuf.GetLayout().GetD3DLayout(), pvsbc));

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
		float specularIntensity = 0.8f;
		float specularPower = 40.0f;
		float padding[3];
	} pmc;

	ppBinds.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(pmc, 1u));
	return std::make_unique<Mesh>(std::move(ppBinds));
}
std::unique_ptr<MeshNode> MeshModel::ParseNode(int& nextId, const aiNode& node) noexcept
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

	auto pNode = std::make_unique<MeshNode>(nextId, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}
	return pNode;
}
