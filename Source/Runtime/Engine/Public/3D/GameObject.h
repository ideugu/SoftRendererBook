#pragma once

namespace CK
{
namespace DDD
{

enum class GameObjectType : UINT32
{
	Normal = 0,
	Gizmo
};

class GameObject
{
public:
	GameObject(const std::string& InName) : _Name(InName)
	{
		_Hash = std::hash<std::string>()(_Name);
	}

	~GameObject() {}

public:
	// ����
	void SetGameObjectType(const GameObjectType& _InGameObjectType) { _GameObjectType = _InGameObjectType; }
	FORCEINLINE bool IsGizmoObject() const { return _GameObjectType == GameObjectType::Gizmo; }

	// Ʈ������
	Transform& GetTransform() { return _Transform; }
	const Transform& GetTransform() const { return _Transform; }
	void SetParent(GameObject& InGameObject) { _Transform.SetParent(InGameObject.GetTransform()); }

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

	// ������ ����
	bool IsVisible() const { return _IsVisible; }
	void SetVisible(bool InVisible) { _IsVisible = InVisible; }

private:
	GameObjectType _GameObjectType = GameObjectType::Normal;
	bool _IsVisible = true;
	std::size_t _Hash = Math::InvalidHash;
	std::string _Name;
	std::size_t _MeshKey = Math::InvalidHash;
	Transform _Transform;
	LinearColor _Color = LinearColor::Error;
};



}
}