
#include "Precompiled.h"
#include "SoftRenderer.h"

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
	static float rotateSpeed = 180.f;

	InputManager input = _GameEngine.GetInputManager();

	// �÷��̾� ���� ������Ʈ�� Ʈ������
	Transform2D& transform = _GameEngine.GetCamera()->GetTransform();
	transform.AddPosition(Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds);

	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;
}

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	// ī�޶��� �� ���
	Matrix3x3 viewMat = _GameEngine.GetCamera()->GetViewMatrix();

	// �÷��̾� ���� ������Ʈ�� Ʈ������
	Transform2D& transform = _GameEngine.GetPlayer()->GetTransform();
	Matrix3x3 finalMat = viewMat * transform.GetModelingMatrix();

	// ���� ������Ʈ Ʈ������ ������ ���
	_RSI->PushStatisticTexts(finalMat.ToStrings());

	// �÷��̾� ���� ������Ʈ�� �޽�
	const Mesh* mesh = _GameEngine.GetPlayer()->GetMesh();

	size_t vertexCount = mesh->_Vertices.size();
	size_t indexCount = mesh->_Indices.size();
	size_t triangleCount = indexCount / 3;

	// �������� ����� ���� ���ۿ� �ε��� ���� ����
	Vector2* vertices = new Vector2[vertexCount];
	std::memcpy(vertices, &mesh->_Vertices[0], sizeof(Vector2) * vertexCount);
	int* indices = new int[indexCount];
	std::memcpy(indices, &mesh->_Indices[0], sizeof(int) * indexCount);

	// �� ������ ����� ����
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

	delete[] vertices;
	delete[] indices;
}

