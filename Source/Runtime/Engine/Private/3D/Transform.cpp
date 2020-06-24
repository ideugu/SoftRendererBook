
#include "Precompiled.h"
using namespace CK::DDD;

Matrix4x4 Transform::GetModelingMatrix() const
{
	Matrix4x4 translate = Matrix4x4(Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4(_Position, true));
	Matrix4x4 rotate = Matrix4x4(Vector4(_Right, false), Vector4(_Up, false), Vector4(_Forward, false), Vector4::UnitW);
	Matrix4x4 scale = Matrix4x4(Vector4::UnitX * _Scale.X, Vector4::UnitY * _Scale.Y, Vector4::UnitZ * _Scale.Z, Vector4::UnitW);
	return translate * rotate * scale;
}

void Transform::CalculateLocalAxis()
{
	float cy, sy, cp, sp, cr, sr;
	Math::GetSinCos(sy, cy, _Rotation.Yaw);
	Math::GetSinCos(sp, cp, _Rotation.Pitch);
	Math::GetSinCos(sr, cr, _Rotation.Roll);

	_Right = Vector3(cy * cr + sy * sp * sr, cp * sr, -sy * cr + cy * sp * sr);
	_Up = Vector3(-cy * sr + sy * sp * cr, cp * cr, sy * sr + cy * sp * cr);
	_Forward = Vector3(sy * cp, -sp, cy * cp);
}
