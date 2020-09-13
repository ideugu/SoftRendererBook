
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
	static float shearSpeed = 2.f;
	static float scaleSpeed = 10.f;
	static float rotateSpeed = 180.f;

	// ���� ��⿡�� �Է� ������ ��������
	InputManager input = _GameEngine.GetInputManager();
	float deltaShear = input.GetXAxis() * shearSpeed * InDeltaSeconds;
	float deltaScale = input.GetZAxis() * scaleSpeed * InDeltaSeconds;
	float deltaRotation = input.GetWAxis() * rotateSpeed * InDeltaSeconds;

	_CurrentShear += deltaShear;
	_CurrentScale = Math::Clamp(_CurrentScale + deltaScale, 5.f, 15.f);
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

	static const Vector2 pivot(200.f, 0.f);

	// ũ�� ���
	Vector2 sBasis1(_CurrentScale, 0.f);
	Vector2 sBasis2(0.f, _CurrentScale);
	Matrix2x2 sMat(sBasis1, sBasis2);

	// ȸ�� ���
	float sin, cos;
	Math::GetSinCos(sin, cos, _CurrentDegree);
	Vector2 rBasis1(cos, sin);
	Vector2 rBasis2(-sin, cos);
	Matrix2x2 rMat(rBasis1, rBasis2);

	// �б� ���
	Vector2 shBasis1 = Vector2::UnitX;
	Vector2 shBasis2(_CurrentShear, 1.f);
	Matrix2x2 shMat(shBasis1, shBasis2);

	// �ռ� ���
	Matrix2x2 cMat = shMat * rMat * sMat;

	// ũ�� ����� �����
	float invScale = 1.f / _CurrentScale;
	Vector2 isBasis1(invScale, 0.f);
	Vector2 isBasis2(0.f, invScale);
	Matrix2x2 isMat(isBasis1, isBasis2);

	// ȸ�� ����� �����
	Vector2 irBasis1(cos, -sin);
	Vector2 irBasis2(sin, cos);
	Matrix2x2 irMat(irBasis1, irBasis2);

	// �б� ����� �����
	Vector2 ishBasis1 = Vector2::UnitX;
	Vector2 ishBasis2(-_CurrentShear, 1.f);
	Matrix2x2 ishMat(ishBasis1, ishBasis2);

	// ������� �ռ����. ( �������� �����ϱ� )
	Matrix2x2 icMat = isMat * irMat * ishMat;

	// �� �� �ʱ�ȭ
	rad = 0.f;
	HSVColor hsv(0.f, 1.f, 0.85f); // �� ���̵��� ä���� ���ݸ� �ٿ���. 

	for (auto const& v : hearts)
	{
		hsv.H = rad / Math::TwoPI;

		// ���� ��Ʈ ( ��ȯ ����� �����ϱ� )
		Vector2 left = cMat * v;
		_RSI->DrawPoint(left - pivot, hsv.ToLinearColor());

		// ������ ��Ʈ ( ��ȯ ���� ������� �����ϱ� )
		Vector2 right = icMat * left;
		_RSI->DrawPoint(right + pivot, hsv.ToLinearColor());

		rad += increment;
	}

	// ���� ��ġ�� �������� ȭ�鿡 ���
	_RSI->PushStatisticText(std::string("Shear : ") + std::to_string(_CurrentShear));
	_RSI->PushStatisticText(std::string("Scale : ") + std::to_string(_CurrentScale));
	_RSI->PushStatisticText(std::string("Rotation : ") + std::to_string(_CurrentDegree));
}

