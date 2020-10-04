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
	// 트랜스폼
	Transform& GetTransform() { return _Transform; }
	const Transform& GetTransform() const { return _Transform; }
	void SetParent(GameObject& InGameObject) { _Transform.SetParent(InGameObject.GetTransform()); }

	// 메시
	void SetMesh(const std::size_t& InMeshKey) { _MeshKey = InMeshKey; }
	bool HasMesh() const { return _MeshKey != Math::DefaultHash; }
	FORCEINLINE std::size_t GetMeshKey() const { return _MeshKey; }

	// 색상
	void SetColor(const LinearColor& InColor) { _Color = InColor; }
	FORCEINLINE const LinearColor& GetColor() const { return _Color; }

	// 키 관련
	const std::string& GetName() const { return _Name; }
	std::size_t GetHash() const { return _Hash; }

	// 검색 관련
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