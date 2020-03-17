
#include "Precompiled.h"
#include "GameEngine.h"

bool GameEngine::Init()
{
	if (_InputManager.GetXAxis && _InputManager.GetYAxis && _InputManager.SpacePressed)
	{
		return true;
	}

	return false;
}
