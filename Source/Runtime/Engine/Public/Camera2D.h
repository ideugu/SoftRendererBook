#pragma once

class Camera2D
{
public:
	Camera2D() { }
	~Camera2D() { }

public:
	Transform2D& GetTransform() { return _Transform; }
	Matrix3x3 GetViewMatrix() const;
	void SetCameraViewSize(const ScreenPoint& InScreenSize);
	void SetCameraCircleBound(float InRadius) { CircleBound.Radius = InRadius; }
	const Circle& GetCircleBound() const { return CircleBound; }
	const Rectangle& GetRectangleBounds() const { return RectangleBound; }

protected:
	Transform2D _Transform;

	Circle CircleBound;
	Rectangle RectangleBound;
};

