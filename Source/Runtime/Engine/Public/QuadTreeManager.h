#pragma once

namespace CK
{

class QuadTreeManager
{
public:
	QuadTreeManager() { }

	Vector2 Center() { return Vector2::Zero; }
	void Build();

	std::unique_ptr<UINT32[]> _QuadTreeArray;
};

}