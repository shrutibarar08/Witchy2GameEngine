#pragma once

#include "EngineManager/System/MeshLoader.h"
#include "EngineManager/System/MeshLoader.h"


class W2PlayerCharacter
{
public:
	//~ Need Mesh Character
	void Draw() noexcept;
	void InitControlUI(float deltaTime) noexcept;
	//~ Need to way to use translation on whole character by key inputs
	//~ Need to focus camera on player
	//~ Need walking animation
private:
	MeshModel m_playerMesh{ "Assets\\Models\\3d_monster_kitten\\scene.gltf", ModelWindow::TRANSFORM_PARAM_DESC
		{
			0, 0, 1.6f, -15, 0, 1.6
	}};
};
