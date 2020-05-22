#pragma once

class Camera2D
{
public:
	Camera2D() = default;

public:
	Transform2D& GetTransform() { return _Transform; }
	Matrix3x3 GetViewMatrix() const;

protected:
	Transform2D _Transform;
};

