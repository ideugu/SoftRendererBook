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
		_BindPose = InTransform;
		_TransformNode.SetLocalTransform(InTransform);
	}

public:
	// Ʈ������
	TransformNode& GetTransformNode() { return _TransformNode; }
	const TransformNode& GetTransformNode() const { return _TransformNode; }
	const Transform& GetBindPose() const { return _BindPose; }
	void SetParent(Bone& InBone) 
	{
		_ParentName = InBone.GetName();
		_TransformNode.SetParent(InBone.GetTransformNode());
	}

	// Ű ����
	const std::string& GetName() const { return _Name; }
	std::size_t GetHash() const { return _Hash; }

private:
	std::size_t _Hash = 0;
	std::string _Name;
	TransformNode _TransformNode;

	// ���� ������ ��ǥ�� ���
	Transform _BindPose;

	// �θ� ����
	std::string _ParentName;
};

}
}