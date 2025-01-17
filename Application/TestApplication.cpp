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
	W2WindowAPI::Get()->EnableCursor();

	for (int i = 0; i < 10; i++)
	{
		std::unique_ptr<MeshModel> x = std::make_unique<MeshModel>("Assets\\Models\\projectile\\scene.gltf",
			ModelWindow::TRANSFORM_PARAM_DESC(), true);
		m_bullets.emplace_back(std::move(x));
	}
}

void TestApplication::BeginPlay()
{}

void TestApplication::Tick(float deltaTime)
{
	m_light.Bind();
	playerCharacter.Draw();
	m_ground.Draw();
	for (int i = 0; i < 10; i++)
	{
		m_bullets[i]->Draw();
	}
	m_light.Draw(deltaTime);
}

void TestApplication::DebugUI()
{
	//WitchyEngine::DebugUI();
	//m_light.InitControlWindow();
	// m_bullet.ShowWindow(m_timer.DeltaTime());
	for (int i = 0; i < 10; i++)
	{
		m_bullets[i]->Launch(m_timer.DeltaTime());
	}
	playerCharacter.InitControlUI(m_timer.DeltaTime());
}
