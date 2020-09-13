
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
	static float duration = 3.f;
	static float rotateSpeed = 180.f;

	// 경과 시간에 따른 현재 각과 이를 사용한 [0,1]값의 생성
	_CurrentTime += InDeltaSeconds;
	_CurrentTime = Math::FMod(_CurrentTime, duration);
	float currentRad = (_CurrentTime / duration) * Math::TwoPI;
	float alpha = (sinf(currentRad) + 1) * 0.5f;

	// [0,1]을 활용해 주기적으로 크기를 반복하기
	_CurrentScale = Math::Lerp(scaleMin, scaleMax, alpha);

	// 엔진 모듈에서 입력 관리자 가져오기
	InputManager input = _GameEngine.GetInputManager();
	float deltaRotation = input.GetWAxis() * rotateSpeed * InDeltaSeconds;
	Vector2 deltaPosition = Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds;

	_CurrentPosition += deltaPosition;
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

	// 아핀 변환 행렬 ( 크기 ) 
	Vector3 sBasis1(_CurrentScale, 0.f, 0.f);
	Vector3 sBasis2(0.f, _CurrentScale, 0.f);
	Vector3 sBasis3 = Vector3::UnitZ;
	Matrix3x3 sMat(sBasis1, sBasis2, sBasis3);

	// 아핀 변환 행렬 ( 회전 ) 
	float sin, cos;
	Math::GetSinCos(sin, cos, _CurrentDegree);
	Vector3 rBasis1(cos, sin, 0.f);
	Vector3 rBasis2(-sin, cos, 0.f);
	Vector3 rBasis3 = Vector3::UnitZ;
	Matrix3x3 rMat(rBasis1, rBasis2, rBasis3);

	// 아핀 변환 행렬 ( 이동 ) 
	Vector3 tBasis1 = Vector3::UnitX;
	Vector3 tBasis2 = Vector3::UnitY;
	Vector3 tBasis3(_CurrentPosition.X, _CurrentPosition.Y, 1.f);
	Matrix3x3 tMat(tBasis1, tBasis2, tBasis3);

	// 모든 아핀 변환의 조합 행렬
	Matrix3x3 cMat = tMat * rMat * sMat;

	// 각 값 초기화
	rad = 0.f;
	HSVColor hsv(0.f, 1.f, 0.85f); // 잘 보이도록 채도를 조금만 줄였음. 
	for (auto const& v : hearts)
	{
		Vector2 target = cMat * v;
		hsv.H = rad / Math::TwoPI;
		_RSI->DrawPoint(target, hsv.ToLinearColor());
		rad += increment;
	}

	// 현재 위치와 스케일을 화면에 출력
	_RSI->PushStatisticText(std::string("Position : ") + _CurrentPosition.ToString());
	_RSI->PushStatisticText(std::string("Scale : ") + std::to_string(_CurrentScale));
	_RSI->PushStatisticText(std::string("Time : ") + std::to_string(_CurrentTime));
	_RSI->PushStatisticText(std::string("Rotation : ") + std::to_string(_CurrentDegree));
}

