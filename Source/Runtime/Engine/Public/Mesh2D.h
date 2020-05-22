#pragma once

class Mesh2D
{
public:
	Mesh2D() {};
	~Mesh2D() {};

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

