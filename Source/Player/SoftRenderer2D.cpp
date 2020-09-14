
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>

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
	static std::random_device rd;
	static std::mt19937 mt(rd());
	static bool isMoving = true;
	static float duration = 3.f;

	// 플레이어의 시야각 절반 코사인 값
	static float cos = cosf(Math::Deg2Rad(_SightAngle) * 0.5f);
	// 플레이어의 시야 벡터 ( Y축 )
	static Vector2 f = Vector2::UnitY;

	_CurrentTime += InDeltaSeconds;

	// 플레이어의 상태 머신
	if (isMoving)
	{
		if (_CurrentTime >= duration)
		{
			isMoving = false;
			_CurrentTime = 0.f;
			_CurrentPosition = _Destination;
			return;
		}

		float ratio = _CurrentTime / duration;
		_CurrentPosition = Vector2(
			Math::Lerp(_Start.X, _Destination.X, ratio),
			Math::Lerp(_Start.Y, _Destination.Y, ratio)
		);
	}
	else
	{
		if (_CurrentTime >= duration)
		{
			isMoving = true;
			_CurrentTime = 0.f;
			_Start = _CurrentPosition;
			std::uniform_real_distribution<float>  randomPosX(-200.f, 200.f);
			std::uniform_real_distribution<float>  randomPosY(20.f, 200.f);
			_Destination = Vector2(randomPosX(mt), randomPosY(mt));
			return;
		}
	}

	// 플레이어에서 목표물로 가는 정규화된 벡터
	Vector2 v = (_CurrentPosition - _GuardPosition).Normalize();

	// 시야에 감지됨
	if (v.Dot(f) >= cos)
	{
		_PlayerColor = LinearColor::Red;
		_GuardColor = LinearColor::Red;
	}
	else
	{
		_PlayerColor = LinearColor::Gray;
		_GuardColor = LinearColor::Blue;
	}
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	// 플레이어와 보초를 위한 구체
	static float radius = 10.f;
	static std::vector<Vector2> sphere;
	if (sphere.empty())
	{
		for (float x = -radius; x <= radius; ++x)
		{
			for (float y = -radius; y <= radius; ++y)
			{
				Vector2 target(x, y);
				float sizeSquared = target.SizeSquared();
				float rr = radius * radius;
				if (sizeSquared < rr)
				{
					sphere.push_back(target);
				}
			}
		}
	}

	static float length = 300.f;
	float halfAngle = Math::HalfPI - Math::Deg2Rad(_SightAngle) * 0.5f;
	static float sin = sinf(halfAngle);
	static float cos = cosf(halfAngle);

	_RSI->DrawLine(_GuardPosition, _GuardPosition + Vector2(length * cos, length * sin), _GuardColor);
	_RSI->DrawLine(_GuardPosition, _GuardPosition + Vector2(-length * cos, length * sin), _GuardColor);
	_RSI->DrawLine(_GuardPosition, Vector2(0.f, 50.f), LinearColor::Black);

	for (auto const& v : sphere)
	{
		_RSI->DrawPoint(v + _GuardPosition, _GuardColor);
		_RSI->DrawPoint(v + _CurrentPosition, _PlayerColor);
	}

	// 현재 위치와 스케일을 화면에 출력
	_RSI->PushStatisticText(std::string("Time : ") + std::to_string(_CurrentTime));
	_RSI->PushStatisticText(std::string("Target Position : ") + _CurrentPosition.ToString());
}

