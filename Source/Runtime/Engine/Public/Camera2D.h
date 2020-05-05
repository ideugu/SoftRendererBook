#pragma once

class Camera2D : public GameObject2D
{
public:
	Camera2D() = default;
	Matrix3x3 GetViewMatrix() const;
};

