#pragma once

class Camera2D : public GameObject2D
{
public:
	Camera2D()
	{
		_Name = "Main Camera";
		_Hash = std::hash<std::string>()(_Name);
	}
	~Camera2D() { }

	Matrix3x3 GetViewMatrix() const;
};

