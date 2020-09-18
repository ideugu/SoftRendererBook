
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

// �׸��� �׸���
void SoftRenderer::DrawGrid2D()
{
	// �׸��� ����
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// ���� ���� ���
	Vector2 viewPos = _GameEngine2.GetMainCamera().GetTransform().GetPosition();
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
	static float moveSpeed = 300.f;
	static float rotateSpeed = 180.f;
	static float scaleSpeed = 180.f;

	InputManager input = _GameEngine2.GetInputManager();

	// �÷��̾� ���� ������Ʈ�� Ʈ������
	Transform& playerTransform = _GameEngine2.FindGameObject(GameEngine::PlayerKey).GetTransform();
	playerTransform.AddPosition(Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds);
	playerTransform.AddRotation(input.GetWAxis() * rotateSpeed * InDeltaSeconds);
	float newScale = Math::Clamp(playerTransform.GetScale().X + scaleSpeed * input.GetZAxis() * InDeltaSeconds, 10.f, 30.f);
	playerTransform.SetScale(Vector2::One * newScale);

	// �÷��̾ ����ٴϴ� ī�޶��� Ʈ������
	static float thresholdDistance = 1.f;
	Transform& cameraTransform = _GameEngine2.GetMainCamera().GetTransform();
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

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	// ��ü �׸� ��ü�� ��
	size_t totalObjectCount = _GameEngine2.GetScene().size();

	// ī�޶��� �� ���
	Matrix3x3 viewMat = _GameEngine2.GetMainCamera().GetViewMatrix();

	// �����ϰ� ������ ��� ���� ������Ʈ��
	for (auto it = _GameEngine2.SceneBegin(); it != _GameEngine2.SceneEnd(); ++it)
	{
		// ���� ������Ʈ�� �ʿ��� ���� ������ ��������
		GameObject& gameObject = *it->get();
		if (!gameObject.HasMesh())
		{
			continue;
		}

		const Mesh& mesh = _GameEngine2.GetMesh(gameObject.GetMeshKey());
		Transform& transform = gameObject.GetTransform();
		Matrix3x3 finalMat = viewMat * transform.GetModelingMatrix();

		size_t vertexCount = mesh._Vertices.size();
		size_t indexCount = mesh._Indices.size();
		size_t triangleCount = indexCount / 3;

		// �������� ����� ���� ���ۿ� �ε��� ���� ����
		Vector2* vertices = new Vector2[vertexCount];
		std::memcpy(vertices, &mesh._Vertices[0], sizeof(Vector2) * vertexCount);
		int* indices = new int[indexCount];
		std::memcpy(indices, &mesh._Indices[0], sizeof(int) * indexCount);

		// �� ������ ����� ����
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi] = finalMat * vertices[vi];
		}

		// ��ȯ�� ������ �մ� �� �׸���
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi = ti * 3;
			_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 1]], gameObject.GetColor());
			_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 2]], gameObject.GetColor());
			_RSI->DrawLine(vertices[indices[bi + 1]], vertices[indices[bi + 2]], gameObject.GetColor());
		}

		delete[] vertices;
		delete[] indices;
	}

	Transform& playerTransform = _GameEngine2.FindGameObject(GameEngine::PlayerKey).GetTransform();
	
	_RSI->PushStatisticText("Total Game Objects : " + std::to_string(totalObjectCount));
	_RSI->PushStatisticText("Player Position : " + playerTransform.GetPosition().ToString());
	_RSI->PushStatisticText("Player Rotation : " + std::to_string(playerTransform.GetRotation()) + " (deg)");
	_RSI->PushStatisticText("Player Scale : " + std::to_string(playerTransform.GetScale().X));
}

