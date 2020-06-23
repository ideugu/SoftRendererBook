
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

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

	// 플레이어 게임 오브젝트의 트랜스폼
	Transform& transform = _GameEngine.GetPlayer()->GetTransform();
	transform.AddRotation(-input.GetXAxis() * rotateSpeed * InDeltaSeconds);
	transform.AddPosition(transform.GetLocalY() * input.GetYAxis() * moveSpeed * InDeltaSeconds);
	transform.SetScale(Vector2::One * squareScale);

	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	// 플레이어 게임 오브젝트의 트랜스폼
	Transform& transform = _GameEngine.GetPlayer()->GetTransform();
	Matrix3x3 finalMat = transform.GetModelingMatrix();

	// 게임 오브젝트 트랜스폼 정보의 출력
	_RSI->PushStatisticTexts(finalMat.ToStrings());

	// 플레이어 게임 오브젝트의 메시
	const Mesh* mesh = _GameEngine.GetPlayer()->GetMesh();

	size_t vertexCount = mesh->_Vertices.size();
	size_t indexCount = mesh->_Indices.size();
	size_t triangleCount = indexCount / 3;

	// 렌더러가 사용할 정점 버퍼와 인덱스 버퍼 생성
	Vector2* vertices = new Vector2[vertexCount];
	std::memcpy(vertices, &mesh->_Vertices[0], sizeof(Vector2) * vertexCount);
	int* indices = new int[indexCount];
	std::memcpy(indices, &mesh->_Indices[0], sizeof(int) * indexCount);

	// 각 정점에 행렬을 적용
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

	delete[] vertices;
	delete[] indices;
}

