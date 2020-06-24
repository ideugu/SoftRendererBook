
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

// �׸��� �׸���
void SoftRenderer::DrawGrid2D()
{
	// �׸��� ����
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// ���� ���� ���
	Vector2 viewPos = _GameEngine.GetCamera()->GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// ���� �ϴܿ������� ���� �׸���
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// �׸��尡 ���۵Ǵ� ���ϴ� ��ǥ �� ���
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

	// ������ ����
	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	_RSI->DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	_RSI->DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}


// ���� ����
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	static float moveSpeed = 100.f;

	InputManager input = _GameEngine.GetInputManager();

	// �÷��̾� ���� ������Ʈ�� Ʈ������
	Transform& playerTransform = _GameEngine.GetPlayer()->GetTransform();
	playerTransform.AddPosition(Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds);

	// �÷��̾ ����ٴϴ� ī�޶��� Ʈ������
	static float thresholdDistance = 1.f;
	Transform& cameraTransform = _GameEngine.GetCamera()->GetTransform();
	cameraTransform.SetPosition(playerTransform.GetPosition());
}

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	// ��� ��ġ
	size_t totalObjectCount = _GameEngine.GetGameObjects().size();
	size_t culledObjectByCircleCount = 0;
	size_t culledObjectByRectangleCount = 0;
	size_t renderingObjectCount = 0;

	// ī�޶��� �� ���
	Matrix3x3 viewMat = _GameEngine.GetCamera()->GetViewMatrix();

	// ī�޶��� ���� ����
	const Circle& cameraCircleBound = _GameEngine.GetCamera()->GetCircleBound();
	const CK::Rectangle& cameraRectangleBound = _GameEngine.GetCamera()->GetRectangleBounds();

	_RSI->PushStatisticText("Total Count : " + std::to_string(totalObjectCount));

	// �����ϰ� ������ ��� ���� ������Ʈ��
	for (auto it = _GameEngine.GoBegin(); it != _GameEngine.GoEnd(); ++it)
	{
		GameObject* gameObject = it->get();
		const Mesh* mesh = _GameEngine.GetMesh(gameObject->GetMeshKey());
		Transform& transform = gameObject->GetTransform();
		Matrix3x3 finalMat = viewMat * transform.GetModelingMatrix();

		// ���� ������Ʈ�� �浹 ����
		Circle gameObjectCircleBound(mesh->GetCircleBound());
		CK::Rectangle gameObjectRectangleBound(mesh->GetRectangleBound());

		// �浹 ������ �� ��ǥ��� ��ȯ
		gameObjectCircleBound.Center = finalMat * gameObjectCircleBound.Center;
		gameObjectCircleBound.Radius = gameObjectCircleBound.Radius * transform.GetScale().Max();

		gameObjectRectangleBound.Min = finalMat * gameObjectRectangleBound.Min;
		gameObjectRectangleBound.Max = finalMat * gameObjectRectangleBound.Max;

		// ī�޶� �ٿ�� ������ �浹 üũ
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

		// �������� ����� ���� ���ۿ� �ε��� ���� ����
		Vector2* vertices = new Vector2[vertexCount];
		std::memcpy(vertices, mesh->_Vertices.data(), sizeof(Vector2) * vertexCount);
		int* indices = new int[indexCount];
		std::memcpy(indices, mesh->_Indices.data(), sizeof(int) * indexCount);

		// �� ������ ����� ����
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi] = finalMat * vertices[vi];
		}

		// ��ȯ�� ������ �մ� �� �׸���
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

