#pragma once

namespace CK
{
namespace DDD
{

class Bone
{
public:
	Bone() = default;
	Bone(const std::string& InName, const Transform& InTransform) : _Name(InName)
	{
		_Hash = std::hash<std::string>()(_Name);
		_BindPose = _Transform;
	}

public:
	// 트랜스폼
	Transform& GetTransform() { return _Transform; }
	const Transform& GetTransform() const { return _Transform; }
	const Transform& GetBindPose() const { return _BindPose; }

	// 키 관련
	const std::string& GetName() const { return _Name; }
	std::size_t GetHash() const { return _Hash; }

private:
	std::size_t _Hash = 0;
	std::string _Name;
	Transform _Transform;
	Transform _BindPose;
};

}
}