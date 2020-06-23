
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

// 그리드 그리기
void SoftRenderer::DrawGrid2D()
{
	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 뷰의 영역 계산
	Vector2 viewPos = _GameEngine.GetCamera()->GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// 좌측 하단에서부터 격자 그리기
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// 그리드가 시작되는 좌하단 좌표 값 계산
	Vector2 minPos = viewPos - extent;
	Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
	ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

	for (int ix = 0; ix < xGridCount; ++ix)
	{
		_RSI->DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy)
	{
		_RSI->DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	// 월드의 원점
	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	_RSI->DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	_RSI->DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}


// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	static float moveSpeed = 100.f;

	InputManager input = _GameEngine.GetInputManager();

	// 플레이어 게임 오브젝트의 트랜스폼
	Transform& playerTransform = _GameEngine.GetPlayer()->GetTransform();
	playerTransform.AddPosition(Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds);

	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;

	// 플레이어를 따라다니는 카메라의 트랜스폼
	static float thresholdDistance = 1.f;
	Transform& cameraTransform = _GameEngine.GetCamera()->GetTransform();
	Vector2 playerPosition = playerTransform.GetPosition();
	Vector2 prevCameraPosition = cameraTransform.GetPosition();
	if ((playerPosition - prevCameraPosition).SizeSquared() < thresholdDistance * thresholdDistance)
	{
		cameraTransform.SetPosition(playerPosition);
	}
	else
	{
		static float lerpSpeed = 2.f;
		float ratio = lerpSpeed * InDeltaSeconds;
		ratio = Math::Clamp(ratio, 0.f, 1.f);
		Vector2 newCameraPosition = prevCameraPosition + (playerPosition - prevCameraPosition) * ratio;
		cameraTransform.SetPosition(newCameraPosition);
	}
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	// 카메라의 뷰 행렬
	Matrix3x3 viewMat = _GameEngine.GetCamera()->GetViewMatrix();

	// 플레이어 게임 오브젝트의 트랜스폼
	Transform& transform = _GameEngine.GetPlayer()->GetTransform();
	Matrix3x3 finalMat = viewMat * transform.GetModelingMatrix();

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

