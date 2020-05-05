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
	const Mesh& GetMesh() { return _QuadMesh; }

private:
	InputManager _InputManager;
	Mesh _QuadMesh;
};

