#pragma once

#include "WindowSystem/W2Window.h"
#include "Utils/W2Timer.h"

/**
 * @brief Test Application Class
 */
class TestApplication
{
public:
	TestApplication();

	/**
	 * @brief Executes Test Application (Application Entry Point)
	 */
	int Execute();

private:
	void UpdateFrame();
	void ComputeFPS();

private:
	W2Window m_window;
	W2Timer  m_timer{};
	int m_updateCount{ 0 };
	float m_updateDelta{ 0 };
};
