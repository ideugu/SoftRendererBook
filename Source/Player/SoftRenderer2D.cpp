
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

// �׸��� �׸���
void SoftRenderer::DrawGrid2D()
{
	// �׸��� ����
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// ���� ���� ���� �׸���
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

	// ���� �� �׸���
	_RSI->DrawFullHorizontalLine(screenHalfSize.Y, LinearColor::Red);
	_RSI->DrawFullVerticalLine(screenHalfSize.X, LinearColor::Green);
}


// ���� ����
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	static float moveSpeed = 100.f;
	static float rotateSpeed = 180.f;
	static float scaleSpeed = 180.f;

	InputManager input = _GameEngine.GetInputManager();

	// �÷��̾� ���� ������Ʈ�� Ʈ������
	Transform& playerTransform = _GameEngine.FindGameObject(GameEngine::PlayerKey).GetTransform();
	playerTransform.AddPosition(Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds);
	playerTransform.AddRotation(input.GetWAxis() * rotateSpeed * InDeltaSeconds);
	float newScale = Math::Clamp(playerTransform.GetScale().X + scaleSpeed * input.GetZAxis() * InDeltaSeconds, 10.f, 30.f);
	playerTransform.SetScale(Vector2::One * newScale);
}

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	// ��ü �׸� ��ü�� ��
	size_t totalObjectCount = _GameEngine.GetScene().size();

	// �����ϰ� ������ ��� ���� ������Ʈ��
	for (auto it = _GameEngine.SceneBegin(); it != _GameEngine.SceneEnd(); ++it)
	{
		// ���� ������Ʈ�� �ʿ��� ���� ������ ��������
		GameObject& gameObject = *it->get();
		if (!gameObject.HasMesh())
		{
			continue;
		}

		const Mesh& mesh = _GameEngine.GetMesh(gameObject.GetMeshKey());
		Transform& transform = gameObject.GetTransform();
		Matrix3x3 finalMat = transform.GetModelingMatrix();

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

	Transform& playerTransform = _GameEngine.FindGameObject(GameEngine::PlayerKey).GetTransform();
	
	_RSI->PushStatisticText("Total Game Objects : " + std::to_string(totalObjectCount));
	_RSI->PushStatisticText("Player Position : " + playerTransform.GetPosition().ToString());
	_RSI->PushStatisticText("Player Rotation : " + std::to_string(playerTransform.GetRotation()) + " (deg)");
	_RSI->PushStatisticText("Player Scale : " + std::to_string(playerTransform.GetScale().X));
}

