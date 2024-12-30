#include "W2Camera.h"

#include <memory>
#include <stdexcept>

W2Camera* W2Camera::m_instance = nullptr;

void W2Camera::Init(const W2Camera_INIT_DESC& desc) noexcept
{
	if (m_instance == nullptr)
	{
		m_instance = new W2Camera(desc);
	}
}

W2Camera* W2Camera::Get()
{
	if (m_instance != nullptr)
	{
		return m_instance;
	}
	throw std::logic_error("Called W2Camera without initializing it!");
}

void W2Camera::Release()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = nullptr;
	}
}

DirectX::XMMATRIX W2Camera::GetTransformMatrix() const noexcept
{
	return mWorldMatrix * mProjectionMatrix * mViewMatrix;
}

DirectX::XMMATRIX W2Camera::GetWorldMatrix() const noexcept
{
	return mWorldMatrix;
}

DirectX::XMMATRIX W2Camera::GetViewMatrix() const noexcept
{
	return mProjectionMatrix;
}

DirectX::XMMATRIX W2Camera::GetProjectionMatrix() const noexcept
{
	return mViewMatrix;
}

W2Camera::W2Camera(const W2Camera_INIT_DESC& desc)
{
	mWorldMatrix = desc.WorldMatrix;
	mProjectionMatrix = desc.ProjectionMatrix;
	mViewMatrix = desc.ViewMatrix;
}
