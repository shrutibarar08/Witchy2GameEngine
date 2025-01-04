#pragma once

#include "EngineManager/WitchyEngine.h"
#include "RenderManager/Object/Drawable.h"
#include "EngineManager/Light/W2PointLight.h"

/**
 * @brief Test Application Class
 */
class TestApplication: public WitchyEngine
{
public:
	TestApplication();

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void DebugUI() override;

private:
	std::vector<std::unique_ptr<Drawable>> m_objects;
	std::vector<std::string> m_availableTextures;
	std::string currentTexture;
	float m_speedFactor{ 1.f };
	int m_meshCount{ 100 };
	W2PointLight m_light{};
};
