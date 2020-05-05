#pragma once

class GameEngine
{
public:
	GameEngine() = default;

public:
	bool Init();
	bool LoadResources();
	bool LoadScene();
	InputManager& GetInputManager() { return _InputManager; }
	GameObject2D* GetPlayer() { return _Player.get(); }

private:
	InputManager _InputManager;
	std::unique_ptr<Mesh> _QuadMesh;
	std::unique_ptr<GameObject2D> _Player;
};

