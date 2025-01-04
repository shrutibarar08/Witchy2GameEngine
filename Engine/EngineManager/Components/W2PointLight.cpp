#include "W2PointLight.h"
#include "ImuGui/imgui.h"


W2PointLight::W2PointLight(float radius)
	: m_sphereMesh(radius)
{}

void W2PointLight::InitControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_translation.x, -100.f, 100.f, "%.1f");
		ImGui::SliderFloat("Y", &m_translation.y, -100.f, 100.f, "%.1f");
		ImGui::SliderFloat("Z", &m_translation.z, -100.f, 100.f, "%.1f");
	}
	ImGui::End();
}

void W2PointLight::Draw() const noexcept
{
	m_sphereMesh.SetTranslation(m_translation);
	m_sphereMesh.Draw();
}

void W2PointLight::Bind() const noexcept
{
	m_lightBuffer.Update(LightBuffer{ m_translation });
	m_lightBuffer.Bind();
}
