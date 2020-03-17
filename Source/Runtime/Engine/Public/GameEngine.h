#pragma once

#include "InputManager.h"

class GameEngine
{
public:
	GameEngine() = default;

public:
	bool Init();
	InputManager& GetInputManager() { return _InputManager; }

private:
	InputManager _InputManager;
};

