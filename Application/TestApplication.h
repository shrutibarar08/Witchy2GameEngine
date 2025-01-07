#pragma once

#include "EngineManager/WitchyEngine.h"
#include "EngineManager/Light/W2PointLight.h"
#include "EngineManager/System/MeshLoader.h"

/**
 * @brief Test Application Class
 */
class TestApplication final : public WitchyEngine
{
public:
	TestApplication();

	void BeginPlay() override;
	void Tick(float deltaTime) override;
	void DebugUI() override;

private:
	float m_speedFactor{ 1.f };
	int m_meshCount{ 100 };

	//~ Objects
	W2PointLight m_light{};
	MeshModel m_meshLoader{ "Assets/Models/octopus_toy.obj" };

	struct
	{
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	} m_transform;
};
