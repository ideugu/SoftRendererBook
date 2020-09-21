#pragma once

namespace CK
{
namespace DDD
{

class Mesh
{
public:
	Mesh() {};
	~Mesh() {};

	bool HasUV() const { return _UVs.size() > 0; }

public:
	std::vector<Vector3> _Vertices;
	std::vector<int> _Indices;
	std::vector<Vector2> _UVs;
};

}
}