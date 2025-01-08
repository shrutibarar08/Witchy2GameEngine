#include "W2PointLight.h"

#include "EngineManager/Player/W2Camera.h"
#include "ImuGui/imgui.h"


W2PointLight::W2PointLight(float radius)
	:m_sphereMesh(radius), m_lightBuffer()
{
}

void W2PointLight::InitControlWindow() noexcept
{
	if (ImGui::Begin("Light Properties"))
	{
		ImGui::Text("Transform");
		ImGui::SliderFloat("X", &m_data.LightPosition.x, -100.f, 100.f, "%.1f");
		ImGui::SliderFloat("Y", &m_data.LightPosition.y, -100.f, 100.f, "%.1f");
		ImGui::SliderFloat("Z", &m_data.LightPosition.z, -100.f, 100.f, "%.1f");

		ImGui::Text("Properties");
		ImGui::ColorEdit3("Ambient", &m_data.Ambient.x);
		ImGui::ColorEdit3("Diffuse Color", &m_data.DiffuseColor.x);

		ImGui::SliderFloat("Diffuse Intensity", &m_data.DiffuseIntensity, 0.0f, 1.0f);
		ImGui::SliderFloat("Constant attenuation", &m_data.AttConst, 0.0f, 10.0f);
		ImGui::SliderFloat("Linear attenuation", &m_data.AttLin, 0.0f, 2.0f);
		ImGui::SliderFloat("Quadratic attenuation", &m_data.AttQuad, 0.0f, 1.0f, "%.5f", 8);
	}
	ImGui::End();
}

void W2PointLight::Draw(float deltaTime) noexcept
{
	m_data.LightPosition.x += deltaTime * 5;
	if (m_data.LightPosition.x >= 65) m_data.LightPosition.x = -70.f;
	m_sphereMesh.SetTranslation(m_data.LightPosition);
	m_sphereMesh.Draw();
}

void W2PointLight::Bind() const noexcept
{
	auto data = m_data;
	const auto translation = DirectX::XMLoadFloat3(&m_data.LightPosition);
	DirectX::XMStoreFloat3(&data.LightPosition, DirectX::XMVector3Transform(translation, W2Camera::Get()->GetViewMatrix()));
	m_lightBuffer.Update(LightBuffer{ data});
	m_lightBuffer.Bind();
}
