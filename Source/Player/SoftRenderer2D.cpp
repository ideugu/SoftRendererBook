
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
	static float duration = 3.f;

	// �÷��̾��� �þ߰� ���� �ڻ��� ��
	static float cos = cosf(Math::Deg2Rad(_SightAngle) * 0.5f);
	// �÷��̾��� �þ� ���� ( Y�� )
	static Vector2 f = Vector2::UnitY;

	_CurrentTime += InDeltaSeconds;

	// �÷��̾��� ���� �ӽ�
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

	// �÷��̾�� ��ǥ���� ���� ����ȭ�� ����
	Vector2 v = (_CurrentPosition - _GuardPosition).Normalize();

	// �þ߿� ������
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

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	// �÷��̾�� ���ʸ� ���� ��ü
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

	// ���� ��ġ�� �������� ȭ�鿡 ���
	_RSI->PushStatisticText(std::string("Time : ") + std::to_string(_CurrentTime));
	_RSI->PushStatisticText(std::string("Target Position : ") + _CurrentPosition.ToString());
}

