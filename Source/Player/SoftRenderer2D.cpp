
#include "Precompiled.h"
#include "SoftRenderer.h"

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
	static float squareScale = 100.f;

	InputManager input = _GameEngine.GetInputManager();

	_Transform.AddRotation(-input.GetXAxis() * rotateSpeed * InDeltaSeconds);
	_Transform.AddPosition(_Transform.GetLocalY() * input.GetYAxis() * moveSpeed * InDeltaSeconds);
	_Transform.SetScale(Vector2::One * squareScale);

	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;
}

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	// Ʈ������ ������ ���
	_RSI->PushStatisticText(_Transform.GetPosition().ToString());
	_RSI->PushStatisticText(std::to_string(_Transform.GetRotation()));
	_RSI->PushStatisticText(_Transform.GetScale().ToString());

	Matrix3x3 finalMat = _Transform.GetModelingMatrix();

	// ���� �޽� �����͸� �������� �ʰ� ���ο� �޽� �����͸� ������ ����.
	size_t vertexCount = _GameEngine.GetMeshPtr()->_Vertices.size();
	size_t indexCount = _GameEngine.GetMeshPtr()->_Indices.size();
	size_t triangleCount = indexCount / 3;

	Vector2* newVertices = new Vector2[vertexCount];
	std::memcpy(newVertices, &_GameEngine.GetMeshPtr()->_Vertices[0], sizeof(Vector2) * vertexCount);
	int* newIndice = new int[indexCount];
	std::memcpy(newIndice, &_GameEngine.GetMeshPtr()->_Indices[0], sizeof(int) * indexCount);

	// �� ������ ����� ����
	for (int vi = 0; vi < vertexCount; ++vi)
	{
		newVertices[vi] = finalMat * newVertices[vi];
	}

	// ��ȯ�� ������ �մ� �� �׸���
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

