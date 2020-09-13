
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
	// ���� �������� ����ϴ� ����
	static float rotateSpeed = 180.f;

	// ���� ��⿡�� �Է� ������ ��������
	InputManager input = _GameEngine.GetInputManager();
	float deltaRotation = input.GetWAxis() * rotateSpeed * InDeltaSeconds;

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

	// �������� 50���� ��ġ �����ϱ�
	const int offsetCount = 50;
	const float range = 250.f;
	static std::random_device rd;
	static std::mt19937 generator(rd());
	static std::uniform_real_distribution<float> dist(-range, range);
	static std::vector<Vector2> offsets;
	if (offsets.empty())
	{
		for (int ix = 0; ix < offsetCount; ++ix)
		{
			offsets.push_back(Vector2(dist(generator), dist(generator)));
		}
	}

	// ȸ�� ��ȯ�� ������ ���
	float sin, cos;
	Math::GetSinCos(sin, cos, _CurrentDegree);

	// ���� ��ȯ�� ���� ����
	Vector2 basis1(cos, sin);
	Vector2 basis2(-sin, cos);

	// ���� ��ȯ�� ���� ���͸� ����� ȸ�� ���
	Matrix2x2 rMat(basis1, basis2);

	for (int ix = 0; ix < offsetCount; ++ix)
	{
		for (auto const& v : hearts)
		{
			Vector2 target = rMat * v;
			hsv.H = rad / Math::TwoPI;
			_RSI->DrawPoint(target + offsets[ix], hsv.ToLinearColor());
			rad += increment;
		}
	}

	// ���� ȸ�� ���� ȭ�鿡 ���
	_RSI->PushStatisticText(std::string("Rotation : ") + std::to_string(_CurrentDegree));
}

