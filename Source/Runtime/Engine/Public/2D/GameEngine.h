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
	InputManager& GetInputManager() { return _InputManager; }
	const DD::Mesh2D& GetMesh() { return _QuadMesh; }

private:
	ScreenPoint _ViewportSize;
	InputManager _InputManager;
	DD::Mesh2D _QuadMesh;
};

}
}