#pragma once

namespace CK
{
namespace DDD
{

class Bone
{
public:
	Bone() = default;
	Bone(const std::string& InName, const TransformData& InTransform) : _Name(InName), _ParentName(InName)
	{
		_Hash = std::hash<std::string>()(_Name);
		_BindPose = InTransform;
		_Transform.SetLocalTransform(InTransform);
	}

public:
	// 트랜스폼
	Transform& GetTransform() { return _Transform; }
	const Transform& GetTransform() const { return _Transform; }
	const TransformData& GetBindPose() const { return _BindPose; }
	void SetParent(Bone& InBone) 
	{
		_ParentName = InBone.GetName();
		_Transform.SetParent(InBone.GetTransform());
	}

	// 키 관련
	const std::string& GetName() const { return _Name; }
	std::size_t GetHash() const { return _Hash; }
	bool HasParent() const { return _Name.compare(_ParentName) != 0; }
	const std::string& GetParentName() const { return _ParentName; }

private:
	std::size_t _Hash = 0;
	std::string _Name;
	Transform _Transform;

	// 로컬 공간의 좌표로 기록
	TransformData _BindPose;

	// 부모 정보
	std::string _ParentName;
};

}
}