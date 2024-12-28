#pragma once

#include "WindowsManager/W2Window.h"
#include "RenderManager/W2RenderAPI.h"
#include "W2Timer.h"


/*
 * @brief The main engine class for the Witchy Engine
 * listens to events and redirect them to the right place
 */
class WitchyEngine
{
public:
	// TODO: Create Config file for the engine
	WitchyEngine(RECT rt, const char* gameName);
	virtual ~WitchyEngine() = default;

	int Execute();
	void UpdateFrame();

	virtual void BeginPlay() = 0;
	virtual void Tick(float deltaTime) = 0;

private:
	void ComputeFPS();

private:
	W2Timer  m_timer{};
	W2Window m_window;

	//~ Logic members
	int   m_updateCount{ 0 };
	float m_accumulatedDelta{ 0.0f };
};
