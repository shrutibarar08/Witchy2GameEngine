#pragma once

#include "EngineManager/WitchyEngine.h"
#include "RenderManager/Object/Drawable.h"

/**
 * @brief Test Application Class
 */
class TestApplication: public WitchyEngine
{
public:
	TestApplication();

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

private:
	std::vector<std::unique_ptr<Drawable>> m_objects;
	float m_speedFactor{ 1.f };
};
