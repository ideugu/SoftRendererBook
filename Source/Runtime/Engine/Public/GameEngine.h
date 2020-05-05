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
	Camera2D* GetCamera() { return _Camera.get(); }

private:
	InputManager _InputManager;
	std::unique_ptr<Mesh> _QuadMesh;
	std::unique_ptr<GameObject2D> _Player;
	std::unique_ptr<Camera2D> _Camera;
};

