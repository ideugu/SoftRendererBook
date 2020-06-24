#pragma once

namespace CK
{
namespace DD
{

class GameEngine
{
public:
	GameEngine() = default;

public:
	bool Init(const ScreenPoint& InViewportSize);
	bool LoadResources();
	bool LoadScene();
	InputManager& GetInputManager() { return _InputManager; }
	DD::GameObject* GetPlayer() { return _Player.get(); }
	DD::Camera* GetCamera() { return _Camera.get(); }

private:
	ScreenPoint _ViewportSize;
	InputManager _InputManager;
	std::unique_ptr<DD::Mesh> _QuadMesh;
	std::unique_ptr<DD::GameObject> _Player;
	std::unique_ptr<DD::Camera> _Camera;
};

}
}