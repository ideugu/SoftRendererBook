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
	bool IsSkinnedMesh() const { return _MeshType == MeshType::Skinned; }
	bool HasUV() const { return _UVs.size() > 0; }
	bool HasBone(const std::string& InBoneName) const { return _Bones.find(InBoneName) != _Bones.end(); }

	void CalculateBounds();
	const Sphere& GetSphereBound() const { return _SphereBound; }
	const Box& GetBoxBound() const { return _BoxBound; }

	// 리깅 관련 함수
	Transform& GetBoneTransform(const std::string& InBoneName) { return _Bones.at(InBoneName).GetTransform(); }
	const Transform& GetBoneTransform(const std::string& InBoneName) const { return _Bones.at(InBoneName).GetTransform(); }
	const Transform& GetBindPose(const std::string& InBoneName) const { return _Bones.at(InBoneName).GetBindPose(); }

public:
	std::vector<Vector3> _Vertices;
	std::vector<int> _Indices;
	std::vector<Vector2> _UVs;

	std::vector<BYTE> _ConnectedBones;
	std::vector<Weight> _Weights;
	std::unordered_map<std::string, Bone> _Bones;

private:
	MeshType _MeshType;
	Sphere _SphereBound;
	Box _BoxBound;
};

}
}