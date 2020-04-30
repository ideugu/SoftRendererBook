
#include "Precompiled.h"
#include "SoftRenderer.h"

// 그리드 그리기
void SoftRenderer::DrawGrid2D()
{
	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 가로 세로 라인 그리기
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

	// 월드 축 그리기
	_RSI->DrawFullHorizontalLine(screenHalfSize.Y, LinearColor::Red);
	_RSI->DrawFullVerticalLine(screenHalfSize.X, LinearColor::Green);
}


// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// 시간에 따른 회전량
	static float rotateSpeed = 180.f;
	float deltaRadian = Math::Deg2Rad(rotateSpeed * InDeltaSeconds);

	// 행렬 설계
	float sin = sinf(deltaRadian);
	float cos = cosf(deltaRadian);
	Vector2 xAxis(cos, sin);
	Vector2 yAxis(-sin, cos);
	Matrix2x2 rotateMat(xAxis, yAxis);

	// 점 회전하기
	_CurrentPosition = rotateMat * _CurrentPosition;
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	// 지정된 위치에 지정한 색상으로 점 찍기
	_RSI->DrawPoint(_CurrentPosition, _CurrentColor);
	_RSI->DrawPoint(_CurrentPosition + Vector2::UnitX, _CurrentColor);
	_RSI->DrawPoint(_CurrentPosition - Vector2::UnitX, _CurrentColor);
	_RSI->DrawPoint(_CurrentPosition + Vector2::UnitY, _CurrentColor);
	_RSI->DrawPoint(_CurrentPosition - Vector2::UnitY, _CurrentColor);

	_RSI->PushStatisticText(_CurrentPosition.ToString());
}

