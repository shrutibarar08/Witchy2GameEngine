#include "W2PlayerCharacter.h"
#include "WindowsManager/W2WindowAPI.h"


void W2PlayerCharacter::Draw() noexcept
{
	m_playerMesh.Draw();
}

void W2PlayerCharacter::InitControlUI(float deltaTime) noexcept
{
	m_playerMesh.ShowWindow(deltaTime, nullptr, true);
}
