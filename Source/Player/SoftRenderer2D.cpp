
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
	// 게임 로직에만 사용하는 변수
	static float moveSpeed = 100.f;
	static float scaleMin = 80.f;
	static float scaleMax = 200.f;
	static float scaleSpeed = 50.f;
	static float rotateSpeed = 180.f;

	// 엔진 모듈에서 입력 관리자 가져오기
	InputManager input = _GameEngine.GetInputManager();
	float deltaScale = input.GetZAxis() * scaleSpeed * InDeltaSeconds;
	float deltaRotation = input.GetWAxis() * rotateSpeed * InDeltaSeconds;
	Vector2 deltaPosition = Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds;

	_CurrentScale += deltaScale;
	_CurrentDegree += deltaRotation;
	_CurrentPosition += deltaPosition;
	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;
}

struct Vertex
{
	Vertex(Vector2 InPosition) : Position(InPosition) {}
	Vector2 Position;
};

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	////////////////////// 메시 데이터 //////////////////////
	static const float squareHalfSize = 0.5f;
	static const int vertexCount = 4;
	static const int triangleCount = 2;

	// 정점 배열과 인덱스 배열 생성
	Vertex vertices[vertexCount] = {
		Vertex(Vector2(-squareHalfSize, -squareHalfSize)),
		Vertex(Vector2(-squareHalfSize, squareHalfSize)),
		Vertex(Vector2(squareHalfSize, squareHalfSize)),
		Vertex(Vector2(squareHalfSize, -squareHalfSize))
	};

	static const int indices[triangleCount * 3] = {
		0, 1, 2,
		0, 2, 3
	};

	// 변환 행렬의 설계
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

	// 정점에 행렬을 적용
	for (int vi = 0; vi < vertexCount; ++vi)
	{
		vertices[vi].Position = cMat * vertices[vi].Position;
	}

	for (int ti = 0; ti < triangleCount; ++ti)
	{
		// 삼각형마다 칠하기
		int bi = ti * 3;
		std::vector<Vertex> t = { vertices[indices[bi]] , vertices[indices[bi + 1]], vertices[indices[bi + 2]] };

		Vector2 minPos(Math::Min3(t[0].Position.X, t[1].Position.X, t[2].Position.X), Math::Min3(t[0].Position.Y, t[1].Position.Y, t[2].Position.Y));
		Vector2 maxPos(Math::Max3(t[0].Position.X, t[1].Position.X, t[2].Position.X), Math::Max3(t[0].Position.Y, t[1].Position.Y, t[2].Position.Y));

		// 무게중심좌표를 위한 준비작업
		Vector2 u = t[1].Position - t[0].Position;
		Vector2 v = t[2].Position - t[0].Position;

		// 공통 분모 값을 구할 것. ( uu * vv - uv * uv )
		float udotv = u.Dot(v);
		float vdotv = v.Dot(v);
		float udotu = u.Dot(u);
		float denominator = udotv * udotv - vdotv * udotu;
		if (Math::EqualsInTolerance(denominator, 0.0f))
		{
			continue;
		}
		float invDenominator = 1.f / denominator;

		// 화면상의 점 구하기
		ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
		ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

		// 두 점이 화면 밖을 벗어나는 경우 클리핑 처리
		lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
		lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
		upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
		upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

		// 모든 점을 Loop
		for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
		{
			for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
			{
				ScreenPoint fragment = ScreenPoint(x, y);
				Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
				Vector2 w = pointToTest - t[0].Position;
				float wdotu = w.Dot(u);
				float wdotv = w.Dot(v);

				float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
				float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
				float oneMinusST = 1.f - s - t;
				if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
				{
					_RSI->DrawPoint(pointToTest, LinearColor::Blue);
				}
			}
		}
	}

	// 관련 데이터 화면 출력
	_RSI->PushStatisticText(std::string("Position : ") + _CurrentPosition.ToString());
	_RSI->PushStatisticText(std::string("Rotation : ") + std::to_string(_CurrentDegree));
	_RSI->PushStatisticText(std::string("Scale : ") + std::to_string(_CurrentScale));
}

