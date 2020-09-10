
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
	static float scaleMin = 3.f;
	static float scaleMax = 15.f;
	static float scaleSpeed = 20.f;
	static float rotateSpeed = 180.f;

	// 엔진 모듈에서 입력 관리자 가져오기
	InputManager input = _GameEngine.GetInputManager();
	Vector2 deltaPosition = Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds;
	float deltaScale = input.GetZAxis() * scaleSpeed * InDeltaSeconds;
	float deltaRotation = input.GetWAxis() * rotateSpeed * InDeltaSeconds;

	_CurrentPosition += deltaPosition;
	_CurrentDegree += deltaRotation;
	_CurrentScale = Math::Clamp(_CurrentScale + deltaScale, scaleMin, scaleMax);
	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	static std::vector<Vector2> hearts;
	if (hearts.empty())
	{
		for (float rad = 0.f; rad < Math::TwoPI; rad += 0.001f)
		{
			float sin = sinf(rad);
			float cos = cosf(rad);
			float cos2 = cosf(2 * rad);
			float cos3 = cosf(3 * rad);
			float cos4 = cosf(4 * rad);
			float x = 16.f * sin * sin * sin;
			float y = 13 * cos - 5 * cos2 - 2 * cos3 - cos4;
			hearts.push_back(Vector2(x, y));
		}
	}

	for (auto const& v : hearts)
	{
		float sin, cos;
		Math::GetSinCos(sin, cos, _CurrentDegree);
		Vector2 target = v * _CurrentScale;
		Vector2 rotatedTarget = Vector2(target.X * cos - target.Y * sin, target.X * sin + target.Y * cos);
		rotatedTarget += _CurrentPosition;
		_RSI->DrawPoint(rotatedTarget, _CurrentColor);
	}

	// 현재 위치와 스케일을 화면에 출력
	_RSI->PushStatisticText(std::string("Position : ") + _CurrentPosition.ToString());
	_RSI->PushStatisticText(std::string("Scale : ") + std::to_string(_CurrentScale));
	_RSI->PushStatisticText(std::string("Rotation : ") + std::to_string(_CurrentDegree));
}

