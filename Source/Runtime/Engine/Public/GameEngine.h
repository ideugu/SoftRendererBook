#pragma once

namespace CK
{

class GameEngine
{
public:
	GameEngine() = default;

public:
	bool Init(const ScreenPoint& InViewportSize);
	InputManager& GetInputManager() { return _InputManager; }

private:
	ScreenPoint _ViewportSize;
	InputManager _InputManager;
};

}