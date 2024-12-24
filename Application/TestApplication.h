#pragma once

#include "src/WitchyEngine.h"

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

};
