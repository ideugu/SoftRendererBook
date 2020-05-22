#pragma once

class Mesh2D
{
public:
	Mesh2D() {};
	~Mesh2D() {};

	std::vector<Vector2> _Vertices;
	std::vector<int> _Indices;
};

