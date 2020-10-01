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
	void CalculateBounds();
	const Sphere& GetSphereBound() const { return _SphereBound; }
	const Box& GetBoxBound() const { return _BoxBound; }

public:
	std::vector<Vector3> _Vertices;
	std::vector<int> _Indices;
	std::vector<Vector2> _UVs;

private:
	Sphere _SphereBound;
	Box _BoxBound;
};

}
}