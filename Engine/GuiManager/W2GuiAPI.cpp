#include "W2GuiAPI.h"
#include "ImuGui/imgui.h"
#include <d3d11.h>
#include "ImuGui/imgui_impl_dx11.h"
#include "ImuGui/imgui_impl_win32.h"

#include "RenderManager/W2RenderAPI.h"
#include "WindowsManager/W2WindowAPI.h"

#include <stdexcept>

W2GuiAPI* W2GuiAPI::m_instance = nullptr;

W2GuiAPI::W2GuiAPI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(W2WindowAPI::Get()->GetHandleWindow());
	ImGui_ImplDX11_Init(W2RenderAPI::Get()->GetDevice(), W2RenderAPI::Get()->GetDeviceContext());
}

void W2GuiAPI::Init() noexcept
{
	if (m_instance == nullptr)
	{
		m_instance = new W2GuiAPI();
	}
}

W2GuiAPI* W2GuiAPI::Get()
{
	if (m_instance != nullptr) return m_instance;
	throw std::logic_error("Called Getter for W2GuiAPI when it is not Initialized!");
}

void W2GuiAPI::RecordStart()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void W2GuiAPI::RecordEnd()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void W2GuiAPI::EnableMouseInput()
{
	if (m_instance)
	{
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
	}
}

void W2GuiAPI::DisableMouseInput()
{
	if (m_instance)
	{
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
	}
}

void W2GuiAPI::Release()
{
	ImGui::DestroyContext();
}

