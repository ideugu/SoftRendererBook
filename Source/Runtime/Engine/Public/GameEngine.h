#pragma once

#include "InputManager.h"
#include "Mesh.h"

class GameEngine
{
public:
	GameEngine() = default;

public:
	bool Init();
	bool LoadResources();
	InputManager& GetInputManager() { return _InputManager; }
	const Mesh* GetMeshPtr() { return _MeshPtr.get(); }

private:
	InputManager _InputManager;
	std::unique_ptr<Mesh> _MeshPtr;
};

