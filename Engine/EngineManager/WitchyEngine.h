#pragma once

#include "WindowsManager/W2WindowAPI.h"
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
	virtual ~WitchyEngine();

	int Execute();
	void UpdateFrame();

	virtual void BeginPlay() = 0;
	virtual void Tick(float deltaTime) = 0;
	virtual void DebugUI();

private:
	void ComputeFPS();
	void ShowMouseInputWindow();
	void ListenMouseInput();

private:
	W2Timer  m_timer{};

	//~ Logic members
	int   m_updateCount{ 0 };
	float m_accumulatedDelta{ 0.0f };
	int m_posX{ 0 };
	int m_posY{ 0 };
};
