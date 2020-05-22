#pragma once

class GameEngine
{
public:
	GameEngine() = default;

public:
	bool Init();
	bool LoadResources();
	InputManager& GetInputManager() { return _InputManager; }
	const Mesh2D& GetMesh() { return _QuadMesh; }

private:
	InputManager _InputManager;
	Mesh2D _QuadMesh;
};

