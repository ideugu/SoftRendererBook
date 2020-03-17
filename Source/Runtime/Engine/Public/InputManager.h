#pragma once

#include <functional>

struct InputManager
{
	std::function<float()> GetXAxis;
	std::function<float()> GetYAxis;
	std::function<bool()> SpacePressed;
};
