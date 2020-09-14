
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>

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
	static std::random_device rd;
	static std::mt19937 mt(rd());
	static bool isMoving = true;
	static float duration = 6.f;

	_CurrentTime += InDeltaSeconds;

	// ������ ���� �ӽ�
	if (isMoving)
	{
		if (_CurrentTime >= duration)
		{
			isMoving = false;
			_CurrentTime = 0.f;
			_CurrentDegree = _Destination;
			return;
		}

		float ratio = _CurrentTime / duration;
		_CurrentDegree = Math::Lerp(_Start, _Destination, ratio);
	}
	else
	{
		if (_CurrentTime >= duration)
		{
			isMoving = true;
			_CurrentTime = 0.f;
			std::uniform_real_distribution<float>  randomDegree(0.f, 360.f);
			_Start = _CurrentDegree;
			_Destination = randomDegree(mt);
			return;
		}
	}
}

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	// ���� ��ü
	static std::vector<Vector2> light;
	if (light.empty())
	{
		float lightRadius = 10.f;
		for (float x = -lightRadius; x <= lightRadius; ++x)
		{
			for (float y = -lightRadius; y <= lightRadius; ++y)
			{
				Vector2 target(x, y);
				float sizeSquared = target.SizeSquared();
				float rr = lightRadius * lightRadius;
				if (sizeSquared < rr)
				{
					light.push_back(target);
				}
			}
		}
	}

	// ������ ������ ��ü
	static std::vector<Vector2> sphere;
	if (sphere.empty())
	{
		float sphereRadius = 50.f;
		for (float x = -sphereRadius; x <= sphereRadius; ++x)
		{
			for (float y = -sphereRadius; y <= sphereRadius; ++y)
			{
				Vector2 target(x, y);
				float sizeSquared = target.SizeSquared();
				float rr = sphereRadius * sphereRadius;
				if (sizeSquared < rr)
				{
					sphere.push_back(target);
				}
			}
		}
	}

	float sin, cos;
	static float lightLength = 200.f;
	Math::GetSinCos(sin, cos, _CurrentDegree);
	Vector2 lightPosition = Vector2(cos, sin) * lightLength;
	static HSVColor hsv(0.f, 1.f, 1.f);

	// �ð��� ���� ������ ���ϵ��� ����
	hsv.H = _CurrentTime / 6.f;

	// ���� �׸���
	LinearColor lightColor = hsv.ToLinearColor();
	_RSI->DrawLine(lightPosition, lightPosition - lightPosition.Normalize() * 30.f, LinearColor::Black);
	for (auto const& v : light)
	{
		_RSI->DrawPoint(v + lightPosition, lightColor);
	}

	// ������ �޴� ��ü�� �׸���, �ȼ����� N dot L�� ����� ������ �����ϰ� �̸� ���� ���� �ݿ�
	for (auto const& v : sphere)
	{
		Vector2 n = (v - _SpherePosition).Normalize();
		Vector2 l = (lightPosition - v).Normalize();
		float shading = Math::Clamp(n.Dot(l), 0.f, 1.f);
		_RSI->DrawPoint(v, lightColor * shading);
	}

	// ���� �ð��� ������ ��ġ ���
	_RSI->PushStatisticText(std::string("Time : ") + std::to_string(_CurrentTime));
	_RSI->PushStatisticText(std::string("Light Degree : ") + std::to_string(_CurrentDegree));
}

