
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
	static float rotateSpeed = 180.f;

	// ���� ��⿡�� �Է� ������ ��������
	InputManager input = _GameEngine.GetInputManager();
	float deltaRotation = input.GetWAxis() * rotateSpeed * InDeltaSeconds;
	Vector2 deltaPosition = Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds;

	_CurrentPosition += deltaPosition;
	_CurrentDegree += deltaRotation;
}

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	static float increment = 0.001f;
	float rad = 0.f;
	static std::vector<Vector2> hearts;
	if (hearts.empty())
	{
		for (rad = 0.f; rad < Math::TwoPI; rad += increment)
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

	// �� �� �ʱ�ȭ
	rad = 0.f;
	HSVColor hsv(0.f, 1.f, 0.85f); // �� ���̵��� ä���� ���ݸ� �ٿ���. 
	for (auto const& v : hearts)
	{
		float sin, cos;
		Math::GetSinCos(sin, cos, _CurrentDegree);
		float halfScreenX = _ScreenSize.X * 0.5f;
		float halfScreenY = _ScreenSize.Y * 0.5f;
		Vector2 target = v * _CurrentScale;
		Vector2 ndc = Vector2(target.X / halfScreenX, target.Y / halfScreenY);
		float len = Vector2(ndc.X * _ScreenSize.Y / _ScreenSize.X, ndc.Y).Size();
		Vector2 polarNdc = ndc.ToPolarCoordinate();
		// �������� �־��� ���� ���� ȸ���� �ο���.
		polarNdc.Y += Math::Deg2Rad(_CurrentDegree) * Math::Lerp(0.f, 1.f, len);
		float angle = ndc.Angle() + Math::Deg2Rad(_CurrentDegree) * Math::Lerp(0.f, 1.f, len);
		float radius = ndc.Size();
		target = polarNdc.ToCartesianCoordinate();
		target = Vector2(target.X * halfScreenX, target.Y * halfScreenY);
		Vector2 target2 = Vector2(radius * cosf(angle) * (_ScreenSize.X * 0.5f), radius * sinf(angle) * (_ScreenSize.Y * 0.5f));

		hsv.H = rad / Math::TwoPI;
		_RSI->DrawPoint(target + _CurrentPosition, hsv.ToLinearColor());
		rad += increment;
	}

	// ���� ��ġ�� �������� ȭ�鿡 ���
	_RSI->PushStatisticText(std::string("Position : ") + _CurrentPosition.ToString());
	_RSI->PushStatisticText(std::string("Scale : ") + std::to_string(_CurrentScale));
	_RSI->PushStatisticText(std::string("Rotation : ") + std::to_string(_CurrentDegree));
}

