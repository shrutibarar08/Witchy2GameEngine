#pragma once
#include "RenderManager/Object/WObject.h"
#include "RenderManager/RenderInterface.h"
#include "RenderManager/Core/Vertex.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ExceptionManager/W2Exception.h"
#include "TextLoader.h"

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

// MeshModel
class ModelWindow // pImpl idiom, only defined in this .cpp
{
public:

	struct TRANSFORM_PARAM_DESC
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	ModelWindow(const std::string& name, const TRANSFORM_PARAM_DESC& param = {});
	~ModelWindow() = default;

	void Show(const char* windowName, const MeshNode& root, float deltaTime, bool controller) noexcept;
	void Launch(float deltaTime) noexcept;
	DirectX::XMMATRIX GetTransform() const noexcept;
	MeshNode* GetSelectedNode() const noexcept;

private:
	std::optional<int> selectedIndex;
	MeshNode* pSelectedNode{ nullptr };

	std::unordered_map<int, TRANSFORM_PARAM_DESC> transforms;
	std::string m_name;
	TextFileHandler m_handler{};
	bool m_launched{ false };
	const float m_rightHand{ 24.f };
	const float m_leftHand{ -10.f };
	const float m_top{ -15.f };
	const float m_lower{ 0.0f };
};

class MeshModel
{
public:
	MeshModel(const std::string fileName, const ModelWindow::TRANSFORM_PARAM_DESC& param={}, bool normalize=false);
	void Draw() const;
	void ShowWindow(float deltaTime, const char* windowName = nullptr, bool controller = false) noexcept;
	void Launch(float deltaTime) noexcept;
	~MeshModel() noexcept;
private:
	std::unique_ptr<Mesh> ParseMesh(const aiMesh& mesh, const aiMaterial* const* pMaterial);
	std::unique_ptr<MeshNode> ParseNode(int& nextId, const aiNode& node) noexcept;
private:
	std::unique_ptr<MeshNode> pRoot;
	std::vector<std::unique_ptr<Mesh>> ppMeshes;
	std::unique_ptr<class ModelWindow> pWindow;
	std::string m_basePath{ "" };
	bool m_normalize;
};
