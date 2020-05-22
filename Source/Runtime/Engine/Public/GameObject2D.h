#pragma once

class GameObject2D
{
public:
	GameObject2D() = default;
	GameObject2D(const std::string& InName) : _Name(InName)
	{
		_Hash = std::hash<std::string>()(_Name);
	}

	~GameObject2D() { }

public:
	Transform2D& GetTransform() { return _Transform; }
	void SetMesh(const std::string& InMeshKey);
	const std::string& GetMeshKey() const;
	bool HasMesh() const { return (_MeshKey.size() > 0); }
	void SetColor(const LinearColor& InColor) { _Color = InColor; }
	LinearColor& GetColor() { return _Color; }
	const std::string& GetName() const { return _Name; }

protected:
	std::size_t _Hash = 0;
	std::string _Name;
	std::string _MeshKey;
	Transform2D _Transform;
	LinearColor _Color = LinearColor::Error;
};

