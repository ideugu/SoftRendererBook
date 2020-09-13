
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
	static float shearSpeed = 2.f;
	static float scaleSpeed = 10.f;
	static float rotateSpeed = 180.f;

	// 엔진 모듈에서 입력 관리자 가져오기
	InputManager input = _GameEngine.GetInputManager();
	float deltaShear = input.GetXAxis() * shearSpeed * InDeltaSeconds;
	float deltaScale = input.GetZAxis() * scaleSpeed * InDeltaSeconds;
	float deltaRotation = input.GetWAxis() * rotateSpeed * InDeltaSeconds;

	_CurrentShear += deltaShear;
	_CurrentScale = Math::Clamp(_CurrentScale + deltaScale, 5.f, 15.f);
	_CurrentDegree += deltaRotation;
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
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

	// 크기 행렬
	Vector2 sBasis1(_CurrentScale, 0.f);
	Vector2 sBasis2(0.f, _CurrentScale);
	Matrix2x2 sMat(sBasis1, sBasis2);

	// 회전 행렬
	float sin, cos;
	Math::GetSinCos(sin, cos, _CurrentDegree);
	Vector2 rBasis1(cos, sin);
	Vector2 rBasis2(-sin, cos);
	Matrix2x2 rMat(rBasis1, rBasis2);

	// 밀기 행렬
	Vector2 shBasis1 = Vector2::UnitX;
	Vector2 shBasis2(_CurrentShear, 1.f);
	Matrix2x2 shMat(shBasis1, shBasis2);

	// 합성 행렬
	Matrix2x2 cMat = shMat * rMat * sMat;

	// 크기 행렬의 역행렬
	float invScale = 1.f / _CurrentScale;
	Vector2 isBasis1(invScale, 0.f);
	Vector2 isBasis2(0.f, invScale);
	Matrix2x2 isMat(isBasis1, isBasis2);

	// 회전 행렬의 역행렬
	Vector2 irBasis1(cos, -sin);
	Vector2 irBasis2(sin, cos);
	Matrix2x2 irMat(irBasis1, irBasis2);

	// 밀기 행렬의 역행렬
	Vector2 ishBasis1 = Vector2::UnitX;
	Vector2 ishBasis2(-_CurrentShear, 1.f);
	Matrix2x2 ishMat(ishBasis1, ishBasis2);

	// 역행렬의 합성행렬. ( 역순으로 조합하기 )
	Matrix2x2 icMat = isMat * irMat * ishMat;

	// 각 값 초기화
	rad = 0.f;
	HSVColor hsv(0.f, 1.f, 0.85f); // 잘 보이도록 채도를 조금만 줄였음. 

	for (auto const& v : hearts)
	{
		hsv.H = rad / Math::TwoPI;

		// 왼쪽 하트 ( 변환 행렬을 적용하기 )
		Vector2 left = cMat * v;
		_RSI->DrawPoint(left - pivot, hsv.ToLinearColor());

		// 오른쪽 하트 ( 변환 값에 역행렬을 적용하기 )
		Vector2 right = icMat * left;
		_RSI->DrawPoint(right + pivot, hsv.ToLinearColor());

		rad += increment;
	}

	// 현재 위치와 스케일을 화면에 출력
	_RSI->PushStatisticText(std::string("Shear : ") + std::to_string(_CurrentShear));
	_RSI->PushStatisticText(std::string("Scale : ") + std::to_string(_CurrentScale));
	_RSI->PushStatisticText(std::string("Rotation : ") + std::to_string(_CurrentDegree));
}

