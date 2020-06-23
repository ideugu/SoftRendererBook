
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
	// �ð��� ���� ȸ����
	static float rotateSpeed = 30.f;
	float deltaRadian = Math::Deg2Rad(rotateSpeed * InDeltaSeconds);

	// ��� ����
	float sin = sinf(deltaRadian);
	float cos = cosf(deltaRadian);
	Vector2 xAxis(cos, sin);
	Vector2 yAxis(-sin, cos);
	Matrix2x2 rotateMat(xAxis, yAxis);

	// ������ ��ġ�� ���� �����ϱ�
	_EndPosition = rotateMat * _EndPosition;
	_CurrentColor = LinearColor::Blue;
}

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	// ���� ���۰� �� ������ ȭ�鿡 ���
	_RSI->PushStatisticText(_StartPosition.ToString());
	_RSI->PushStatisticText(_EndPosition.ToString());

	// �극���� �˰������� �� �׸���
	_RSI->DrawLine(_StartPosition, _EndPosition, _CurrentColor);

}