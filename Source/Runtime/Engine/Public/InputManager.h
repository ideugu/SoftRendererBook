#pragma once

namespace CK
{

struct InputManager
{
	std::function<float()> GetXAxis;
	std::function<float()> GetYAxis;
	std::function<float()> GetZAxis;
	std::function<bool()> SpacePressed;
};

}