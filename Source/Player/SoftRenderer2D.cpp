
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
	// 게임 로직에만 사용하는 변수
	static float moveSpeed = 100.f;

	// 엔진 모듈에서 입력 관리자 가져오기
	InputManager input = _GameEngine.GetInputManager();
	Vector2 deltaPosition = Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds;
	_CurrentPosition += deltaPosition;

	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	// 지정한 점을 기준으로 상하좌우로 점 찍기
	_RSI->DrawPoint(_CurrentPosition, _CurrentColor);
	_RSI->DrawPoint(_CurrentPosition + Vector2::UnitX, _CurrentColor);
	_RSI->DrawPoint(_CurrentPosition - Vector2::UnitX, _CurrentColor);
	_RSI->DrawPoint(_CurrentPosition + Vector2::UnitY, _CurrentColor);
	_RSI->DrawPoint(_CurrentPosition - Vector2::UnitY, _CurrentColor);

	_RSI->PushStatisticText(_CurrentPosition.ToString());
}

