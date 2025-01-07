#include "WitchyEngine.h"

#include "RenderManager/W2RenderAPI.h"
#include "GuiManager/W2GuiAPI.h"
#include "Player/W2Camera.h"

#include <sstream>

#include "ImuGui/imgui.h"


WitchyEngine::WitchyEngine(RECT rt, const char* gameName)
{
	// TODO: Create Initializer PQ for this based on Importance and Dependencies.
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
	W2GuiAPI::Init();
}

WitchyEngine::~WitchyEngine()
{
	// TODO: Create PQ for structured release.
	W2Camera::Release();
#ifdef _DEBUG
	W2GuiAPI::Release();
#endif
	W2RenderAPI::Release();
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

	Tick(m_timer.DeltaTime());	// Sends Update Event to Application Inherited this Engine
	ComputeFPS();
#ifdef _DEBUG
	DebugUI();
#endif

	W2Camera::UpdateFreeCamera(m_timer.DeltaTime());

	//~ End Recordings
	W2GuiAPI::Get()->RecordEnd();
	W2RenderAPI::Get()->RecordEnd();
}

void WitchyEngine::DebugUI()
{
	ShowMouseInputWindow();
	W2Camera::Get()->InitControlWindow();

	if (ImGui::Begin("Color Selection"))
	{
		static float color_values[4]{};
		ImGui::SliderFloat("Red", &color_values[0], 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Blue", &color_values[1], 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Green", &color_values[2], 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Alpha", &color_values[3], 0.0f, 1.0f, "%.2f");
		W2RenderAPI::Get()->SetBackgroundColor(color_values);
	}
	ImGui::End();
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

void WitchyEngine::ShowMouseInputWindow()
{
	while (const auto d = W2WindowAPI::Get()->Mouse.ReadMouseDelta())
	{
		m_posX += d->x;
		m_posY += d->y;
	}
	if (ImGui::Begin("Raw Input"))
	{
		ImGui::Text("Tally: (%d,%d)", m_posX, m_posY);
	}
	ImGui::End();
}
