#pragma once
#include "RenderManager/Object/WObject.h"
#include "RenderManager/RenderInterface.h"
#include "RenderManager/Core/Vertex.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ExceptionManager/W2Exception.h"

#include <iostream>

class ModelException : public W2Exception
{
public:
	ModelException(int line, const char* file, std::string note) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;
private:
	std::string note;
};

class Mesh : public WObject<Mesh>
{
public:
	Mesh(std::vector<std::unique_ptr<Bindable>> bindPtrs);
	void Draw(DirectX::FXMMATRIX accumulatedTransform) const;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 m_transform;
};

class MeshNode
{
	friend class MeshModel;
public:
	MeshNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform);
	void Draw(DirectX::FXMMATRIX accumulatedTransform) const;
	void DrawChild(MeshNode*& pSelectedNode) const noexcept;

	void SetTransform(DirectX::FXMMATRIX transform) noexcept;
	int GetId() const noexcept;

private:
	void AddChild(std::unique_ptr<MeshNode> pChild);
private:
	std::string m_name;
	std::vector<std::unique_ptr<MeshNode>> ppChildNodes;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 m_transform;
	DirectX::XMFLOAT4X4 m_appliedTransform;
	int m_id;
};

class MeshModel
{
public:
	MeshModel(const std::string fileName);
	void Draw() const;
	void ShowWindow(const char* windowName = nullptr) noexcept;
	~MeshModel() noexcept;
private:
	static std::unique_ptr<Mesh> ParseMesh(const aiMesh& mesh, const aiMaterial* const* pMaterial);
	std::unique_ptr<MeshNode> ParseNode(int& nextId, const aiNode& node) noexcept;
private:
	std::unique_ptr<MeshNode> pRoot;
	std::vector<std::unique_ptr<Mesh>> ppMeshes;
	std::unique_ptr<class ModelWindow> pWindow;
};
