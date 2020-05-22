
#include "Precompiled.h"
#include "SoftRenderer.h"

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
	Transform2D& playerTransform = _GameEngine.GetPlayer()->GetTransform();
	playerTransform.AddPosition(Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds);

	// 플레이어를 따라다니는 카메라의 트랜스폼
	static float thresholdDistance = 1.f;
	Transform2D& cameraTransform = _GameEngine.GetCamera()->GetTransform();
	cameraTransform.SetPosition(playerTransform.GetPosition());
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	// 통계 수치
	size_t totalObjectCount = _GameEngine.GetGameObjects().size();
	size_t culledObjectByCircleCount = 0;
	size_t culledObjectByRectangleCount = 0;
	size_t renderingObjectCount = 0;

	// 카메라의 뷰 행렬
	Matrix3x3 viewMat = _GameEngine.GetCamera()->GetViewMatrix();

	// 카메라의 가시 영역
	const Circle& cameraCircleBound = _GameEngine.GetCamera()->GetCircleBound();
	const Rectangle& cameraRectangleBound = _GameEngine.GetCamera()->GetRectangleBounds();

	_RSI->PushStatisticText("Total Count : " + std::to_string(totalObjectCount));

	// 랜덤하게 생성된 모든 게임 오브젝트들
	for (auto it = _GameEngine.GoBegin(); it != _GameEngine.GoEnd(); ++it)
	{
		GameObject2D* gameObject = it->get();
		const Mesh2D* mesh = _GameEngine.GetMesh(gameObject->GetMeshKey());
		Transform2D& transform = gameObject->GetTransform();
		Matrix3x3 finalMat = viewMat * transform.GetModelingMatrix();

		// 게임 오브젝트의 충돌 영역
		Circle gameObjectCircleBound(mesh->GetCircleBound());
		Rectangle gameObjectRectangleBound(mesh->GetRectangleBound());

		// 충돌 영역을 뷰 좌표계로 변환
		gameObjectCircleBound.Center = finalMat * gameObjectCircleBound.Center;
		gameObjectCircleBound.Radius = gameObjectCircleBound.Radius * transform.GetScale().Max();

		gameObjectRectangleBound.Min = finalMat * gameObjectRectangleBound.Min;
		gameObjectRectangleBound.Max = finalMat * gameObjectRectangleBound.Max;

		// 카메라 바운딩 영역과 충돌 체크
		if (!cameraCircleBound.Intersect(gameObjectCircleBound))
		{
			culledObjectByCircleCount++;
			continue;
		}

		if (!cameraRectangleBound.Intersect(gameObjectRectangleBound))
		{
			culledObjectByRectangleCount++;
			continue;
		}

		renderingObjectCount++;

		size_t vertexCount = mesh->_Vertices.size();
		size_t indexCount = mesh->_Indices.size();
		size_t triangleCount = indexCount / 3;

		// 렌더러가 사용할 정점 버퍼와 인덱스 버퍼 생성
		Vector2* vertices = new Vector2[vertexCount];
		std::memcpy(vertices, mesh->_Vertices.data(), sizeof(Vector2) * vertexCount);
		int* indices = new int[indexCount];
		std::memcpy(indices, mesh->_Indices.data(), sizeof(int) * indexCount);

		// 각 정점에 행렬을 적용
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi] = finalMat * vertices[vi];
		}

		// 변환된 정점을 잇는 선 그리기
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi = ti * 3;
			_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 1]], gameObject->GetColor());
			_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 2]], gameObject->GetColor());
			_RSI->DrawLine(vertices[indices[bi + 1]], vertices[indices[bi + 2]], gameObject->GetColor());
		}

		delete[] vertices;
		delete[] indices;
	}

	_RSI->PushStatisticText("Culled by Circle Count : " + std::to_string(culledObjectByCircleCount));
	_RSI->PushStatisticText("Culled by Rectangle Count : " + std::to_string(culledObjectByRectangleCount));
	_RSI->PushStatisticText("Rendered Count : " + std::to_string(renderingObjectCount));
}

