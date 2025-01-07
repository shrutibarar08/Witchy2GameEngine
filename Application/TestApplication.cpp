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
#ifdef _DEBUG
	W2WindowAPI::Get()->EnableCursor();
#else
	W2WindowAPI::Get()->DisableCursor();
#endif
}

void TestApplication::BeginPlay()
{}

void TestApplication::Tick(float deltaTime)
{
	m_light.Bind();
	m_meshLoader.Draw();
	m_light.Draw();
}

void TestApplication::DebugUI()
{
	WitchyEngine::DebugUI();
	m_light.InitControlWindow();
	m_meshLoader.ShowWindow();

	static bool showWindow = true;
	if (showWindow)
	{
		ImGui::ShowDemoWindow(&showWindow);
	}
}
