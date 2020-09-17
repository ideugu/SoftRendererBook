
#include "Precompiled.h"
using namespace CK::DD;

const std::string GameEngine::SteveTextureKey("Steve.png");

bool GameEngine::Init(const ScreenPoint& InViewportSize)
{
	_ViewportSize = InViewportSize;

	if (!_InputManager.GetXAxis || !_InputManager.GetYAxis || !_InputManager.GetZAxis || !_InputManager.GetWAxis || !_InputManager.SpacePressed)
	{
		return false;
	}

	_MainTexture = std::make_unique<Texture>(SteveTextureKey);
	if (!GetMainTexture().IsIntialized())
	{
		return false;
	}

	return true;
}
