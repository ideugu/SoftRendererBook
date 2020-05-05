
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
	InputManager input = _GameEngine.GetInputManager();
	Vector2 deltaPosition = Vector2(input.GetXAxis(), input.GetYAxis()) * _MoveSpeed * InDeltaSeconds;
	_PivotPosition += deltaPosition;

	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	////////////////////// 모델링 공간 //////////////////////
	static float squareHalfSize = 0.5f;
	static const int vertexCount = 4;
	static const int triangleCount = 2;

	// 정점 배열과 인덱스 배열 생성
	Vector2 vertices[vertexCount] = {
		Vector2(-squareHalfSize, -squareHalfSize),
		Vector2(-squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, -squareHalfSize)
	};

	int indices[triangleCount * 3] = {
		0, 1, 2,
		0, 2, 3
	};

	////////////////////// 월드 공간 //////////////////////
	// 게임 로직에서 변경한 피벗 위치의 출력
	_RSI->PushStatisticText(_PivotPosition.ToString());

	// 변환 행렬의 설계
	static float squareScale = 100.f;
	Matrix3x3 scaleMat = Matrix3x3(Vector3::UnitX * squareScale, Vector3::UnitY * squareScale, Vector3::UnitZ);
	Matrix3x3 translateMat = Matrix3x3(Vector3::UnitX, Vector3::UnitY, Vector3(_PivotPosition.X, _PivotPosition.Y, 1.f));
	Matrix3x3 finalMat = translateMat * scaleMat;

	// 정점에 행렬을 적용
	for (int vi = 0; vi < vertexCount; ++vi)
	{
		vertices[vi] = finalMat * vertices[vi];
	}

	// 변환된 정점을 잇는 선 그리기
	for (int ti = 0; ti < triangleCount; ++ti)
	{
		int bi = ti * 3;
		_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 1]], _CurrentColor);
		_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 2]], _CurrentColor);
		_RSI->DrawLine(vertices[indices[bi + 1]], vertices[indices[bi + 2]], _CurrentColor);
	}

}

