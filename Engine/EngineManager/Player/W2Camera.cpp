#include "W2Camera.h"

#include <memory>
#include <stdexcept>

#include "ImuGui/imgui.h"

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

void W2Camera::InitControlWindow() noexcept
{
	if (ImGui::Begin("Camera Component"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("R", &m_radius, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &m_theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &m_phi, -89.0f, 89.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &m_roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &m_pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &m_yaw, -180.0f, 180.0f);
	}
	ImGui::End();
}

DirectX::XMMATRIX W2Camera::GetViewMatrix() const noexcept
{
	const auto pos = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(0.0f, 0.0f, -m_radius, 0.0f),
		DirectX::XMMatrixRotationRollPitchYaw(m_phi, -m_theta, 0.0f)
	);
	return DirectX::XMMatrixLookAtLH(
		pos, DirectX::XMVectorZero(),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	) * DirectX::XMMatrixRotationRollPitchYaw(m_pitch, -m_yaw, m_roll);
}

DirectX::XMMATRIX W2Camera::GetWorldMatrix() const noexcept
{
	return mWorldMatrix;
}

DirectX::XMMATRIX W2Camera::GetProjectionMatrix() const noexcept
{
	return mProjectionMatrix;
}

W2Camera::W2Camera(const W2Camera_INIT_DESC& desc)
{
	mWorldMatrix = desc.WorldMatrix;
	mProjectionMatrix = desc.ProjectionMatrix;
}
