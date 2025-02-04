#pragma once
#include "RenderManager/W2RenderAPI.h"
#include "RenderManager/Components/SurfaceTexture.h"
#include <DirectXMath.h>


class Bindable;


class Drawable
{
	template<typename T>
	friend class WObject;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;

	void Draw() const noexcept(!ON_DEBUG);
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	virtual void Update(float dt) noexcept {};
	SurfaceTexture* GetTexture() noexcept;

protected:
	void AddBind(std::unique_ptr<Bindable> bind) noexcept(!ON_DEBUG);
	void AddTexture(std::unique_ptr<SurfaceTexture> textures) noexcept;
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;

	const class IndexBuffer* pIndexBuffer = nullptr;
	std::unique_ptr<SurfaceTexture> pSurfaceTextures = nullptr;
	std::vector<std::unique_ptr<Bindable>> m_binds;
};
