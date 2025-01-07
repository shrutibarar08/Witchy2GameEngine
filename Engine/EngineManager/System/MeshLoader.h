#pragma once
#include "RenderManager/Object/WObject.h"
#include "RenderManager/RenderInterface.h"
#include "RenderManager/Core/Vertex.h"

#include <assimp/scene.h>

class Mesh : public WObject<Mesh>
{
public:
	Mesh(std::vector<std::unique_ptr<Bindable>> ppBinds);
	void Draw(DirectX::FXMMATRIX transformations) const noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 m_transform;
};

class MeshNode
{
	friend class MeshModel;
public:
	MeshNode(std::vector<Mesh*> ppMesh, const DirectX::XMMATRIX& transform) noexcept;
	void Draw(DirectX::FXMMATRIX transformations) const noexcept;
private:
	void AddChild(std::unique_ptr<MeshNode> pChild) noexcept;
private:
	std::vector<std::unique_ptr<MeshNode>> m_ppNode;
	std::vector<Mesh*> m_ppMesh;
	DirectX::XMFLOAT4X4 m_transform;
};

class MeshModel
{
public:
	MeshModel(const std::string fileName);
	static std::unique_ptr<Mesh> ParseMesh(const aiMesh& mesh);
	std::unique_ptr<MeshNode> ParseNode(const aiNode& node);
	void Draw() const;
private:
	std::unique_ptr<MeshNode> m_pRootNode;
	std::vector<std::unique_ptr<Mesh>> m_ppMesh;
};
