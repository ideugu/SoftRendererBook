#pragma once

namespace CK
{
namespace DDD
{

class GameObject
{
public:
	GameObject(const std::string& InName) : _Name(InName)
	{
		_Hash = std::hash<std::string>()(_Name);
	}

	~GameObject() {}

public:
	// Ʈ������
	TransformNode& GetTransformNode() { return _TransformNode; }
	const TransformNode& GetTransformNode() const { return _TransformNode; }
	void SetParent(GameObject& InGameObject) { _TransformNode.SetParent(InGameObject.GetTransformNode()); }

	// �޽�
	void SetMesh(const std::size_t& InMeshKey) { _MeshKey = InMeshKey; }
	bool HasMesh() const { return _MeshKey != Math::InvalidHash; }
	FORCEINLINE std::size_t GetMeshKey() const { return _MeshKey; }

	// ����
	void SetColor(const LinearColor& InColor) { _Color = InColor; }
	FORCEINLINE const LinearColor& GetColor() const { return _Color; }

	// Ű ����
	const std::string& GetName() const { return _Name; }
	std::size_t GetHash() const { return _Hash; }

	// �˻� ����
	bool IsValid() const { return _Hash != Math::InvalidHash; }
	static GameObject Invalid;

private:
	std::size_t _Hash = Math::InvalidHash;
	std::string _Name;
	std::size_t _MeshKey = Math::InvalidHash;
	TransformNode _TransformNode;
	LinearColor _Color = LinearColor::Error;
};



}
}