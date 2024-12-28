#include "WitchyEngine.h"
#include "RenderManager/W2RenderAPI.h"

#include <sstream>


WitchyEngine::WitchyEngine(RECT rt, const char* gameName)
	: m_window(rt, gameName)
{
	//~ Initialize Subsystems
	W2RenderAPI::Init(m_window.GetHandleWindow());	// Init RenderManager (API)
}

int WitchyEngine::Execute()
{
	m_timer.Reset();
	BeginPlay();
	while (true)
	{
		if (const auto ecode = W2Window::ProcessMessages())
		{
			return *ecode;
		}
		m_timer.Tick();
		UpdateFrame();
	}
	return 0;
}

void WitchyEngine::UpdateFrame()
{
	W2RenderAPI::Get()->ClearBuffer();
	Tick(m_timer.DeltaTime());
	ComputeFPS();
	W2RenderAPI::Get()->PresentFrame();

}

void WitchyEngine::ComputeFPS()
{
	m_accumulatedDelta += m_timer.DeltaTime();
	if (m_accumulatedDelta >= 1.0f)
	{
		std::ostringstream oss;
		oss << m_window.GetTitleName() << " - "
			<< "FPS: " << std::to_string(m_updateCount);
		m_window.SetTitleName(oss.str());

		m_accumulatedDelta = 0;
		m_updateCount = 0;
	}
	m_updateCount++;
}
