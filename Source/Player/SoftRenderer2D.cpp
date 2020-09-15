
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
	static float duration = 6.f;
	static float rotateSpeed = 180.f;

	_CurrentTime += InDeltaSeconds;
	_CurrentDegree = Math::FMod(_CurrentDegree + rotateSpeed * InDeltaSeconds, 360.f);
	if (_CurrentTime >= duration)
	{
		std::uniform_real_distribution<float>  randomY(-200.f, 200.f);
		_LineStart = Vector2(-400.f, randomY(mt));
		_LineEnd = Vector2(400.f, randomY(mt));
		_CurrentTime = 0.f;
		return;
	}
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 점
	static std::vector<Vector2> point;
	if (point.empty())
	{
		float radius = 5.f;
		for (float x = -radius; x <= radius; ++x)
		{
			for (float y = -radius; y <= radius; ++y)
			{
				Vector2 target(x, y);
				float sizeSquared = target.SizeSquared();
				float rr = radius * radius;
				if (sizeSquared < rr)
				{
					point.push_back(target);
				}
			}
		}
	}

	// 점 그리기
	float sin, cos;
	static float pointOrbit = 250.f;
	Math::GetSinCos(sin, cos, _CurrentDegree);
	Vector2 pointPos = Vector2(cos, sin) * pointOrbit;
	for (auto const& v : point)
	{
		_RSI->DrawPoint(v + pointPos, LinearColor::Red);
	}

	// 투영할 라인 그리기
	_RSI->DrawLine(_LineStart, _LineEnd, LinearColor::Black);

	// 투영된 위치 그리기
	Vector2 hatV = (_LineEnd - _LineStart).Normalize();
	Vector2 u = pointPos - _LineStart;
	Vector2 projV = hatV * (u.Dot(hatV));
	Vector2 projectedPos = _LineStart + projV;
	float distance = (projectedPos - pointPos).Size();
	for (auto const& v : point)
	{
		_RSI->DrawPoint(v + projectedPos, LinearColor::Magenta);
	}

	// 투영 라인 그리기
	_RSI->DrawLine(projectedPos, pointPos, LinearColor::Gray);

	// 관련 데이터 화면 출력
	_RSI->PushStatisticText(std::string("Time : ") + std::to_string(_CurrentTime));
	_RSI->PushStatisticText(std::string("Point : ") + pointPos.ToString());
	_RSI->PushStatisticText(std::string("Projection : ") + projectedPos.ToString());
	_RSI->PushStatisticText(std::string("Distance : ") + std::to_string(distance));
}

