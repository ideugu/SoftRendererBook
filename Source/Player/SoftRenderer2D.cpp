
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
	static float moveSpeed = 100.0f;

	InputManager input = _GameEngine.GetInputManager();
	Vector2 deltaPosition = Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds;
	_PivotPosition += deltaPosition;

	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;
}

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	////////////////////// �𵨸� ���� //////////////////////
	static float squareHalfSize = 0.5f;
	static const int vertexCount = 4;
	static const int triangleCount = 2;

	// ���� �迭�� �ε��� �迭 ����
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

	////////////////////// ���� ���� //////////////////////
	// ���� �������� ������ �ǹ� ��ġ�� ���
	_RSI->PushStatisticText(_PivotPosition.ToString());

	// ��ȯ ����� ����
	static float squareScale = 100.f;
	Matrix3x3 scaleMat = Matrix3x3(Vector3::UnitX * squareScale, Vector3::UnitY * squareScale, Vector3::UnitZ);
	Matrix3x3 translateMat = Matrix3x3(Vector3::UnitX, Vector3::UnitY, Vector3(_PivotPosition.X, _PivotPosition.Y, 1.f));
	Matrix3x3 finalMat = translateMat * scaleMat;

	// ������ ����� ����
	for (int vi = 0; vi < vertexCount; ++vi)
	{
		vertices[vi] = finalMat * vertices[vi];
	}

	// ��ȯ�� ������ �մ� �� �׸���
	for (int ti = 0; ti < triangleCount; ++ti)
	{
		int bi = ti * 3;
		_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 1]], _CurrentColor);
		_RSI->DrawLine(vertices[indices[bi]], vertices[indices[bi + 2]], _CurrentColor);
		_RSI->DrawLine(vertices[indices[bi + 1]], vertices[indices[bi + 2]], _CurrentColor);
	}

}

