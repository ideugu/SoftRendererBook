
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
	static float moveSpeed = 100.f;
	static float rotateSpeed = 180.f;
	static float squareScale = 100.f;

	InputManager input = _GameEngine.GetInputManager();

	_Transform.AddRotation(-input.GetXAxis() * rotateSpeed * InDeltaSeconds);
	_Transform.AddPosition(_Transform.GetLocalY() * input.GetYAxis() * moveSpeed * InDeltaSeconds);
	_Transform.SetScale(Vector2::One * squareScale);

	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	// 트랜스폼 정보의 출력
	_RSI->PushStatisticText(_Transform.GetPosition().ToString());
	_RSI->PushStatisticText(std::to_string(_Transform.GetRotation()));
	_RSI->PushStatisticText(_Transform.GetScale().ToString());

	Matrix3x3 finalMat = _Transform.GetModelingMatrix();

	// 원본 메시 데이터를 변경하지 않고 새로운 메시 데이터를 복제해 생성.
	size_t vertexCount = _GameEngine.GetMeshPtr()->_Vertices.size();
	size_t indexCount = _GameEngine.GetMeshPtr()->_Indices.size();
	size_t triangleCount = indexCount / 3;

	Vector2* newVertices = new Vector2[vertexCount];
	std::memcpy(newVertices, &_GameEngine.GetMeshPtr()->_Vertices[0], sizeof(Vector2) * vertexCount);
	int* newIndice = new int[indexCount];
	std::memcpy(newIndice, &_GameEngine.GetMeshPtr()->_Indices[0], sizeof(int) * indexCount);

	// 각 정점에 행렬을 적용
	for (int vi = 0; vi < vertexCount; ++vi)
	{
		newVertices[vi] = finalMat * newVertices[vi];
	}

	// 변환된 정점을 잇는 선 그리기
	for (int ti = 0; ti < triangleCount; ++ti)
	{
		int bi = ti * 3;
		_RSI->DrawLine(newVertices[newIndice[bi]], newVertices[newIndice[bi + 1]], _CurrentColor);
		_RSI->DrawLine(newVertices[newIndice[bi]], newVertices[newIndice[bi + 2]], _CurrentColor);
		_RSI->DrawLine(newVertices[newIndice[bi + 1]], newVertices[newIndice[bi + 2]], _CurrentColor);
	}

	delete[] newVertices;
	delete[] newIndice;
}

