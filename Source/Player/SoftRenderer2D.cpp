
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

static std::vector<Vector2> hearts = { Vector2(0.000f, 109.141f),Vector2(0.000f, -109.141f),Vector2(69.421f, 123.000f),Vector2(-69.421f, 123.000f),Vector2(46.344f, 123.000f),Vector2(-46.344f, 123.000f),Vector2(96.000f, 114.000f),Vector2(-96.000f, 114.000f),Vector2(11.294f, 114.000f),Vector2(-11.294f, 114.000f),Vector2(107.103f, 105.000f),Vector2(-107.103f, 105.000f),Vector2(114.000f, 96.000f),Vector2(-114.000f, 96.000f),Vector2(118.523f, 87.000f),Vector2(-118.523f, 87.000f),Vector2(121.412f, 78.000f),Vector2(-121.412f, 78.000f),Vector2(123.053f, 69.000f),Vector2(-123.053f, 69.000f),Vector2(123.676f, 60.000f),Vector2(-123.676f, 60.000f),Vector2(123.432f, 51.000f),Vector2(-123.432f, 51.000f),Vector2(122.421f, 42.000f),Vector2(-122.421f, 42.000f),Vector2(120.715f, 33.000f),Vector2(-120.715f, 33.000f),Vector2(118.361f, 24.000f),Vector2(-118.361f, 24.000f),Vector2(115.394f, 15.000f),Vector2(-115.394f, 15.000f),Vector2(111.836f, 6.000f),Vector2(-111.836f, 6.000f),Vector2(107.697f, -3.000f),Vector2(-107.697f, -3.000f),Vector2(102.979f, -12.000f),Vector2(-102.979f, -12.000f),Vector2(97.674f, -21.000f),Vector2(-97.674f, -21.000f),Vector2(91.765f, -30.000f),Vector2(-91.765f, -30.000f),Vector2(85.221f, -39.000f),Vector2(-85.221f, -39.000f),Vector2(78.000f, -48.000f),Vector2(-78.000f, -48.000f),Vector2(70.039f, -57.000f),Vector2(-70.039f, -57.000f),Vector2(61.247f, -66.000f),Vector2(-61.247f, -66.000f),Vector2(51.496f, -75.000f),Vector2(-51.496f, -75.000f),Vector2(40.585f, -84.000f),Vector2(-40.585f, -84.000f),Vector2(28.194f, -93.000f),Vector2(-28.194f, -93.000f),Vector2(13.740f, -102.000f),Vector2(-13.740f, -102.000f) };

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	for (auto const& v : hearts)
	{
		Vector2 target = v + _CurrentPosition;

		// 지정한 점을 기준으로 상하좌우로 점 찍기
		_RSI->DrawPoint(target, _CurrentColor);
		_RSI->DrawPoint(target + Vector2::UnitX, _CurrentColor);
		_RSI->DrawPoint(target - Vector2::UnitX, _CurrentColor);
		_RSI->DrawPoint(target + Vector2::UnitY, _CurrentColor);
		_RSI->DrawPoint(target - Vector2::UnitY, _CurrentColor);
	}

	Vector2 v = ScreenPoint(400, 300).ToVectorCoordinate(_ScreenSize);
	_RSI->DrawPoint(v, _CurrentColor);
	ScreenPoint p = ScreenPoint::ToScreenCoordinate(_ScreenSize, Vector2(0.9f, 0.1f));
	ScreenPoint p1 = ScreenPoint::ToScreenCoordinate(_ScreenSize, Vector2(0.9f, 0.f));

	// 현재 위치를 화면에 출력
	_RSI->PushStatisticText(_CurrentPosition.ToString());
}

