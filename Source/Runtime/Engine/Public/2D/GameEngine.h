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
	InputManager& GetInputManager() { return _InputManager; }
	Texture& GetMainTexture() { return *_MainTexture.get(); }

	const static std::string SteveTextureKey;

private:
	ScreenPoint _ViewportSize;
	InputManager _InputManager;
	std::unique_ptr<Texture> _MainTexture;
};

}
}