
#include "Precompiled.h"
using namespace CK::DDD;

void Camera::SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp)
{
	Vector3 viewX, viewY, viewZ;

	// 단위 Z축 생성
	viewZ = (InTargetPosition - _TransformNode.GetWorldPosition()).Normalize();
	// 시선 방향과 월드 Y축이 평행한 경우
	if (Math::Abs(viewZ.Y) >= (1.f - SMALL_NUMBER))
	{
		// 특이 상황에서 로컬 X 좌표 값을 임의로 지정.
		viewX = Vector3::UnitX;
	}
	else
	{
		viewX = InUp.Cross(viewZ).Normalize();
	}
	viewY = viewZ.Cross(viewX).Normalize();

	_TransformNode.SetLocalRotation(Matrix3x3(viewX, viewY, viewZ));
}
