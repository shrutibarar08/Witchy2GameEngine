#include "TestApplication.h"

#include <sstream>


TestApplication::TestApplication()
	: m_window({ 0, 0, 1280, 720 }, "Witchy Test Application")
{}

int TestApplication::Execute()
{
	m_timer.Reset();
	
	while (true)
	{
		if (const auto ecode = W2Window::ProcessMessages())
		{
			return *ecode;
		}
		m_timer.Tick();
		UpdateFrame();
	}
	return S_OK;
}

void TestApplication::UpdateFrame()
{
	m_window.RenderAPI().ClearBuffer();
	ComputeFPS();
	m_window.RenderAPI().PresentFrame();
}

void TestApplication::ComputeFPS()
{
	m_updateDelta += m_timer.DeltaTime();
	if (m_updateDelta >= 1.0f)
	{
		std::ostringstream oss;
		oss << m_window.GetTitleName() << " - "
			<< "FPS: " << std::to_string(m_updateCount);
		m_window.SetTitleName(oss.str());

		m_updateCount = 0;
		m_updateDelta = 0;
	}
	m_updateCount++;
}
