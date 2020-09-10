
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
	static float moveSpeed = 100.f;
	static float scaleMin = 20.f;
	static float scaleMax = 50.f;
	static float scaleSpeed = 20.f;

	// ���� ��⿡�� �Է� ������ ��������
	InputManager input = _GameEngine.GetInputManager();
	Vector2 deltaPosition = Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds;
	float deltaScale = input.GetZAxis() * scaleSpeed * InDeltaSeconds;

	_CurrentPosition += deltaPosition;
	_CurrentScale = Math::Clamp(_CurrentScale + deltaScale, scaleMin, scaleMax);
	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;
}

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	static std::vector<Vector2> hearts;
	if (hearts.empty())
	{
		static float range = 10.f;
		static float increment = 0.01f;
		static float n = 10.f;
		static float tolerance = 0.5f;
		for (float y = -range; y < range; y += increment)
		{
			for (float x = -range; x < range; x += increment)
			{
				float test = x * x * y * y * y;
				float u = (x * x + y * y - (n - tolerance));
				float l = (x * x + y * y - (n + tolerance));
				float utest = u * u * u;
				float ltest = l * l * l;
				if(utest > test && ltest < test)
				{
					hearts.push_back(Vector2(x, y));
				}
			}
		}
	}

	for (auto const& v : hearts)
	{
		Vector2 target = (v * _CurrentScale) + _CurrentPosition;
		_RSI->DrawPoint(target, _CurrentColor);
	}

	// ���� ��ġ�� �������� ȭ�鿡 ���
	_RSI->PushStatisticText(std::string("Position : ") + _CurrentPosition.ToString());
	_RSI->PushStatisticText(std::string("Scale : ") + std::to_string(_CurrentScale));
}

