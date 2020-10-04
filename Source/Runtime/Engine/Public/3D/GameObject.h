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
	Transform& GetTransform() { return _Transform; }
	const Transform& GetTransform() const { return _Transform; }
	void SetParent(GameObject& InGameObject) { _Transform.SetParent(InGameObject.GetTransform()); }

	// �޽�
	void SetMesh(const std::size_t& InMeshKey) { _MeshKey = InMeshKey; }
	bool HasMesh() const { return _MeshKey != Math::DefaultHash; }
	FORCEINLINE std::size_t GetMeshKey() const { return _MeshKey; }

	// ����
	void SetColor(const LinearColor& InColor) { _Color = InColor; }
	FORCEINLINE const LinearColor& GetColor() const { return _Color; }

	// Ű ����
	const std::string& GetName() const { return _Name; }
	std::size_t GetHash() const { return _Hash; }

	// �˻� ����
	bool IsValid() const { return _Hash != Math::DefaultHash; }
	const static GameObject NotFound;

private:
	std::size_t _Hash = Math::DefaultHash;
	std::string _Name;
	std::size_t _MeshKey = Math::DefaultHash;
	Transform _Transform;
	LinearColor _Color = LinearColor::Error;
};

}
}