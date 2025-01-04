#include "Drawable.h"
#include "ExceptionManager/RenderAPIMacros.h"
#include "RenderManager/Components/IndexBuffer.h"

#include <cassert>
#include <typeinfo>


void Drawable::Draw() const noexcept(!ON_DEBUG)
{
	for (auto& b: m_binds)
	{
		b->Bind();
	}
	for (auto& b : GetStaticBinds())
	{
		b->Bind();
	}
	if (pSurfaceTextures) pSurfaceTextures->Bind();
	W2RenderAPI::Get()->DrawIndexed(pIndexBuffer->GetCount());
}

SurfaceTexture* Drawable::GetTexture() noexcept
{
	return pSurfaceTextures.get();
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!ON_DEBUG)
{
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	m_binds.push_back(std::move(bind));
}

void Drawable::AddTexture(std::unique_ptr<class SurfaceTexture> textures) noexcept
{
	pSurfaceTextures = std::move(textures);
}

void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept
{
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = ibuf.get();
	m_binds.push_back(std::move(ibuf));
}
