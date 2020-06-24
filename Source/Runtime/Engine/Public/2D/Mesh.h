#pragma once

namespace CK
{
namespace DD
{

class Mesh
{
public:
	Mesh() {};
	~Mesh() {};

public:
	std::vector<Vector2> _Vertices;
	std::vector<int> _Indices;
	void CalculateBounds();
	const Circle& GetCircleBound() const { return CircleBound; }
	const Rectangle& GetRectangleBound() const { return RectangleBound; }

private:
	Circle CircleBound;
	Rectangle RectangleBound;
};

}
}

