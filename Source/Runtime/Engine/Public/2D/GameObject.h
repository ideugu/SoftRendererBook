#pragma once

namespace CK
{
namespace DD
{

class GameObject
{
public:
	GameObject() = default;
	GameObject(const std::string& InName) : _Name(InName)
	{
		_Hash = std::hash<std::string>()(_Name);
	}

	~GameObject() { }

public:
	// Ʈ������
	Transform& GetTransform() { return _Transform; }
	const Transform& GetTransformConst() const { return _Transform; }

	// �޽�
	void SetMesh(const std::string& InMeshKey);
	const std::string& GetMeshKey() const;
	bool HasMesh() const { return _MeshKey.size() > 0; }

	// ����
	void SetColor(const LinearColor& InColor) { _Color = InColor; }
	const LinearColor& GetColor() const { return _Color; }

	// Ű ����
	const std::string& GetName() const { return _Name; }
	std::size_t GetHash() const { return _Hash; }

	// �˻� ����
	bool Found() const { return _Hash == std::hash<std::string>()("!NOTFOUND"); }
	FORCEINLINE bool operator<(const GameObject& InGameObject) const;
	FORCEINLINE bool operator<(const std::string& InString) const;
	FORCEINLINE bool operator==(const GameObject& InGameObject) const;
	FORCEINLINE bool operator==(const std::string& InString) const;
	FORCEINLINE bool operator!=(const GameObject& InGameObject) const;
	FORCEINLINE bool operator!=(const std::string& InString) const;

	const static GameObject NotFound;

private:
	std::size_t _Hash = 0;
	std::string _Name;
	std::string _MeshKey;
	Transform _Transform;
	LinearColor _Color = LinearColor::Error;
};


FORCEINLINE bool GameObject::operator<(const GameObject& InGameObject) const
{
	return _Hash < InGameObject.GetHash();
}

FORCEINLINE bool GameObject::operator<(const std::string& InString) const
{
	return _Hash < std::hash<std::string>()(InString);
}

FORCEINLINE bool GameObject::operator==(const GameObject& InGameObject) const
{
	return _Hash == InGameObject.GetHash();
}

FORCEINLINE bool GameObject::operator==(const std::string& InString) const
{
	return _Hash == std::hash<std::string>()(InString);
}

FORCEINLINE bool GameObject::operator!=(const GameObject& InGameObject) const
{
	return _Hash != InGameObject.GetHash();
}

FORCEINLINE bool GameObject::operator!=(const std::string& InString) const
{
	return _Hash != std::hash<std::string>()(InString);
}

}
}