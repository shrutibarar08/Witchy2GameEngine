#include "TestApplication.h"
#include "Shape/Box.h"
#include "Shape/TestObject.h"
#include "MathLib.h"

#include <memory>
#include <random>
#include <DirectXMath.h>
#include <algorithm>

#include "ImuGui/imgui_impl_dx11.h"

/*
 * @brief Tests Engine Functionality
 */
TestApplication::TestApplication()
	: WitchyEngine({0, 0, 1920, 1080}, 
		"Application_1")
{
}

void TestApplication::BeginPlay()
{

}

void TestApplication::Tick(float deltaTime)
{
	m_light.Bind();
	const auto transform = DirectX::XMMatrixRotationRollPitchYaw(m_transform.roll, m_transform.pitch, m_transform.yaw)
		* DirectX::XMMatrixTranslation(m_transform.x, m_transform.y, m_transform.z);
	m_meshLoader.Draw();
	m_light.Draw();
}

void TestApplication::DebugUI()
{
	WitchyEngine::DebugUI();
	m_light.InitControlWindow();

	if (ImGui::Begin("Mesh"))
	{
		using namespace std::string_literals;

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &m_transform.roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &m_transform.pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &m_transform.yaw, -180.0f, 180.0f);

		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_transform.x, -20.0f, 20.0f);
		ImGui::SliderFloat("Y", &m_transform.y, -20.0f, 20.0f);
		ImGui::SliderFloat("Z", &m_transform.z, -20.0f, 20.0f);
	}
	ImGui::End();
}
