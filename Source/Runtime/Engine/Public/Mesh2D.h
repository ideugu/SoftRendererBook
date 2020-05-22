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
	const Circle& GetCircleBounds() const { return CircleBounds; }

private:
	Circle CircleBounds;
};

