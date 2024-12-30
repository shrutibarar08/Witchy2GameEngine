#include "WitchyEngine.h"

#include "RenderManager/W2RenderAPI.h"
#include "GuiManager/W2GuiAPI.h"
#include "Components/W2Camera.h"

#include <sstream>


WitchyEngine::WitchyEngine(RECT rt, const char* gameName)
{
	//~ Initialize Subsystems

	// Init WindowsAPI
	W2WindowAPI::Init(rt, gameName);

	// Init Engine Components
	W2Camera::W2Camera_INIT_DESC desc{};
	
	GetClientRect(W2WindowAPI::Get()->GetHandleWindow(), &rt);
	const float width = static_cast<float>(rt.right - rt.left);
	const float height = static_cast<float>(rt.bottom - rt.top);
	desc.ProjectionMatrix = DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f);
	desc.WorldMatrix = DirectX::XMMatrixIdentity();

	W2Camera::Init(desc);	// TODO: create a way of saving and passing it as a start position.

	// Init RenderAPI
	W2RenderAPI::Init();	// Init RenderManager (API)
	// Init GuiAPI
#ifdef _DEBUG
	W2GuiAPI::Init();
#endif
}

WitchyEngine::~WitchyEngine()
{
	// TODO: Create PQ for structured release.
	W2RenderAPI::Release();
	W2Camera::Release();
#ifdef _DEBUG
	W2GuiAPI::Release();
#endif
	W2WindowAPI::Release();
}

int WitchyEngine::Execute()
{
	m_timer.Reset();
	BeginPlay();
	while (true)
	{
		if (const auto ecode = W2WindowAPI::ProcessMessages())
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
	// TODO: Create a common abstraction for subsystems (Partially Done)
	// TODO: Create Start and End Recording PQ so that depended class must start recording after less or independent class.
	W2RenderAPI::Get()->RecordStart();
	W2GuiAPI::Get()->RecordStart();

	Tick(m_timer.DeltaTime());	// Sent Update Event to Applications Inherited this Engine
	ComputeFPS();
	DebugUI();

	//~ End Recordings
	W2GuiAPI::Get()->RecordEnd();
	W2RenderAPI::Get()->RecordEnd();
}

void WitchyEngine::DebugUI()
{
	W2Camera::Get()->InitControlWindow();
}

void WitchyEngine::ComputeFPS()
{
	// Compute FPS Based on number of times loop initiated per second.
	m_accumulatedDelta += m_timer.DeltaTime();
	if (m_accumulatedDelta >= 1.0f)
	{
		std::ostringstream oss;
		oss << W2WindowAPI::Get()->GetTitleName() << " - "
			<< "FPS: " << std::to_string(m_updateCount);
		W2WindowAPI::Get()->SetTitleName(oss.str());

		m_accumulatedDelta = 0;
		m_updateCount = 0;
	}
	m_updateCount++;
}
