#pragma once

namespace CK
{
namespace DDD
{

struct Weight
{
	std::vector<std::string> Bones;
	std::vector<float> Values;
};

enum class MeshType : UINT32
{
	Normal = 0,
	Skinned
};

class Mesh
{
public:
	Mesh() = default;

	void SetMeshType(const MeshType& _InMeshType) { _MeshType = _InMeshType; }
	FORCEINLINE bool HasUV() const { return _UVs.size() > 0; }

	void CalculateBounds();
	const Sphere& GetSphereBound() const { return _SphereBound; }
	const Box& GetBoxBound() const { return _BoxBound; }

	// 스켈레탈 애니메이션 관련 함수
	FORCEINLINE bool IsSkinnedMesh() const { return _MeshType == MeshType::Skinned; }
	FORCEINLINE bool HasBone(const std::string& InBoneName) const { return _Bones.find(InBoneName) != _Bones.end(); }

	Bone& GetBone(const std::string& InBoneName) { return _Bones.at(InBoneName); }
	const Bone& GetBone(const std::string& InBoneName) const { return _Bones.at(InBoneName); }
	const TransformData& GetBindPose(const std::string& InBoneName) const { return _Bones.at(InBoneName).GetBindPose(); }

public:
	std::vector<Vector3> _Vertices;
	std::vector<size_t> _Indices;
	std::vector<Vector2> _UVs;

	std::vector<BYTE> _ConnectedBones;
	std::vector<Weight> _Weights;
	std::unordered_map<std::string, Bone> _Bones;

private:
	MeshType _MeshType = MeshType::Normal;
	Sphere _SphereBound;
	Box _BoxBound;
};

}
}