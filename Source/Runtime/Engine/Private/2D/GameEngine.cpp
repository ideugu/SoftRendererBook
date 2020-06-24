
#include "Precompiled.h"
using namespace CK::DD;

bool GameEngine::Init(const ScreenPoint& InViewportSize)
{
	if (!_InputManager.GetXAxis || !_InputManager.GetYAxis || !_InputManager.SpacePressed)
	{
		return false;
	}

	_ViewportSize = InViewportSize;
	return true;
}
