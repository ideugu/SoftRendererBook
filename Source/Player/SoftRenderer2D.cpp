
#include "Precompiled.h"
#include "SoftRenderer.h"

// �׸��� �׸���
void SoftRenderer::DrawGrid2D()
{
	// �׸��� ����
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// ���� ���� ���� �׸���
	ScreenPoint screenHalfSize = _ScreenSize.GetHalf();

	for (int x = screenHalfSize.X; x <= _ScreenSize.X; x += _Grid2DUnit)
	{
		_RSI->DrawFullVerticalLine(x, gridColor);
		if (x > screenHalfSize.X)
		{
			_RSI->DrawFullVerticalLine(2 * screenHalfSize.X - x, gridColor);
		}
	}

	for (int y = screenHalfSize.Y; y <= _ScreenSize.Y; y += _Grid2DUnit)
	{
		_RSI->DrawFullHorizontalLine(y, gridColor);
		if (y > screenHalfSize.Y)
		{
			_RSI->DrawFullHorizontalLine(2 * screenHalfSize.Y - y, gridColor);
		}
	}

	// ���� �� �׸���
	_RSI->DrawFullHorizontalLine(screenHalfSize.Y, LinearColor::Red);
	_RSI->DrawFullVerticalLine(screenHalfSize.X, LinearColor::Green);
}


// ���� ����
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// ���� �������� ����ϴ� ����
	static float duration = 10.f;

	// ��� �ð��� ���� ���� ���� �̸� ����� [0,1]���� ����
	_CurrentTime += InDeltaSeconds;
	_CurrentTime = Math::FMod(_CurrentTime, duration);
	float currentRad = (_CurrentTime / duration) * Math::TwoPI;
	float alpha = (sinf(currentRad) + 1) * 0.5f;

	// [0,1]�� Ȱ���� �ֱ������� ũ�⸦ �ݺ��ϱ�
	_CurrentDegree = Math::Lerp(0.f, 360.f, alpha);
}

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	// �������� ���� ����
	static Vector3 sp(-800.f, 0.f, 1.f);
	static Vector3 ep(800.f, 0.f, 1.f);

	// ���� ��ȯ ��� ( ȸ�� ) 
	float sin, cos;
	Math::GetSinCos(sin, cos, _CurrentDegree);
	Vector3 rBasis1(cos, sin, 0.f);
	Vector3 rBasis2(-sin, cos, 0.f);
	Vector3 rBasis3 = Vector3::UnitZ;
	Matrix3x3 rMat(rBasis1, rBasis2, rBasis3);

	Vector2 s = (rMat * sp).ToVector2();
	Vector2 e = (rMat * ep).ToVector2();
	HSVColor hsv(0.f, 1.f, 0.85f); // �� ���̵��� ä���� ���ݸ� �ٿ���. 
	hsv.H = _CurrentDegree / 360.f;
	_RSI->DrawLine(s, e, hsv.ToLinearColor());

	// ���� ��ġ�� �������� ȭ�鿡 ���
	_RSI->PushStatisticText(std::string("Time : ") + std::to_string(_CurrentTime));
	_RSI->PushStatisticText(std::string("Delta Rotation : ") + std::to_string(_CurrentDegree));
}

