#pragma once

#include "src/WitchyEngine.h"
#include "Renderer/Object/Drawable.h"

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
};
