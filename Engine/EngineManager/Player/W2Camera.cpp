#include "W2Camera.h"

#include <memory>
#include <stdexcept>
#include <algorithm> 

#include "ImuGui/imgui.h"
#include "MathLib.h"

#include "WindowsManager/W2WindowAPI.h"


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

// Updates Camera Location Freely in the scene.
void W2Camera::UpdateFreeCamera(float deltaTime)
{
	while (const auto e = W2WindowAPI::Get()->Keyboard.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}

		switch (e->GetCode())
		{
		case VK_ESCAPE:
			if (W2WindowAPI::Get()->IsCursorEnable())
			{
				W2WindowAPI::Get()->DisableCursor();
			}
			else
			{
				W2WindowAPI::Get()->EnableCursor();
			}
			break;
		}
	}

	if (!W2WindowAPI::Get()->IsCursorEnable())
	{
		if (W2WindowAPI::Get()->Keyboard.KeyIsPressed('W'))
		{
			W2Camera::Get()->Translate({ 0.0f,0.0f,deltaTime });
		}
		if (W2WindowAPI::Get()->Keyboard.KeyIsPressed('A'))
		{
			W2Camera::Get()->Translate({ -deltaTime,0.0f,0.0f });
		}
		if (W2WindowAPI::Get()->Keyboard.KeyIsPressed('S'))
		{
			W2Camera::Get()->Translate({ 0.0f,0.0f,-deltaTime });
		}
		if (W2WindowAPI::Get()->Keyboard.KeyIsPressed('D'))
		{
			W2Camera::Get()->Translate({ deltaTime,0.0f,0.0f });
		}
		if (W2WindowAPI::Get()->Keyboard.KeyIsPressed('R'))
		{
			W2Camera::Get()->Translate({ 0.0f,deltaTime,0.0f });
		}
		if (W2WindowAPI::Get()->Keyboard.KeyIsPressed('F'))
		{
			W2Camera::Get()->Translate({ 0.0f,-deltaTime,0.0f });
		}
	}

	while (const auto delta = W2WindowAPI::Get()->Mouse.ReadMouseDelta())
	{
		if (!W2WindowAPI::Get()->IsCursorEnable())
		{
			W2Camera::Get()->Rotate(static_cast<float>(delta->x), static_cast<float>(delta->y));
		}
	}
}

void W2Camera::InitControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_translation.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &m_translation.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &m_translation.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &m_pitch, -90.0f, 90.0f);
		ImGui::SliderAngle("Yaw", &m_yaw, -180.0f, 180.0f);
	}
	ImGui::End();
}

DirectX::XMMATRIX W2Camera::GetViewMatrix() const noexcept
{
	const DirectX::XMVECTOR forwardBaseVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const auto lookVector = DirectX::XMVector3Transform(forwardBaseVector,
		DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f)
	);
	const auto camPosition = DirectX::XMLoadFloat3(&m_translation);
	const auto camTarget = DirectX::XMVectorAdd(camPosition, lookVector);

	return DirectX::XMMatrixLookAtLH(
		camPosition,
		camTarget,
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

DirectX::XMMATRIX W2Camera::GetWorldMatrix() const noexcept
{
	return mWorldMatrix;
}

DirectX::XMMATRIX W2Camera::GetProjectionMatrix() const noexcept
{
	return mProjectionMatrix;
}

void W2Camera::Rotate(float dx, float dy) noexcept
{
	m_yaw = wrap_angle(m_yaw + dx * m_rotationSpeed);
	m_pitch = std::clamp(m_pitch + dy * m_rotationSpeed, -PI<float> / 2.0f, PI<float> / 2.0f);
}

void W2Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(
		DirectX::XMLoadFloat3(&translation),
		DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f) *
		DirectX::XMMatrixScaling(m_moveSpeed, m_moveSpeed, m_moveSpeed)
	));
	m_translation = {
		m_translation.x + translation.x,
		m_translation.y + translation.y,
		m_translation.z + translation.z
	};
}

W2Camera::W2Camera(const W2Camera_INIT_DESC& desc)
{
	mWorldMatrix = desc.WorldMatrix;
	mProjectionMatrix = desc.ProjectionMatrix;
}
