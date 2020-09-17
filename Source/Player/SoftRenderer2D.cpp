
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>

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
	// ���� �������� ����ϴ� ����
	static float moveSpeed = 100.f;
	static float scaleMin = 80.f;
	static float scaleMax = 200.f;
	static float scaleSpeed = 50.f;
	static float rotateSpeed = 180.f;

	// ���� ��⿡�� �Է� ������ ��������
	InputManager input = _GameEngine.GetInputManager();
	float deltaScale = input.GetZAxis() * scaleSpeed * InDeltaSeconds;
	float deltaRotation = input.GetWAxis() * rotateSpeed * InDeltaSeconds;
	Vector2 deltaPosition = Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds;

	_CurrentScale += deltaScale;
	_CurrentDegree += deltaRotation;
	_CurrentPosition += deltaPosition;
	_CurrentColor = input.SpacePressed() ? LinearColor::Red : LinearColor::Blue;
}

struct Vertex
{
	Vertex() = default;
	Vertex(const Vector2& InPosition, const Vector2& InUV) : Position(InPosition), UV(InUV) {}

	Vector2 Position;
	Vector2 UV;
};

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	////////////////////// �޽� ������ //////////////////////
	static const float squareHalfSize = 0.5f;
	static const int vertexCount = 4;
	static const int triangleCount = 2;

	// ���� �迭�� �ε��� �迭 ����
	Vertex vertices[vertexCount] = {
		Vertex(Vector2(-squareHalfSize, -squareHalfSize), Vector2(0.125f, 0.75f)),
		Vertex(Vector2(-squareHalfSize, squareHalfSize), Vector2(0.125f, 0.875f)),
		Vertex(Vector2(squareHalfSize, -squareHalfSize), Vector2(0.25f, 0.75f)),
		Vertex(Vector2(squareHalfSize, squareHalfSize), Vector2(0.25f, 0.875f))
	};

	static const int indices[triangleCount * 3] = {
		0, 1, 2,
		1, 3, 2
	};

	// ��ȯ ����� ����
	// ���� ��ȯ ��� ( ũ�� ) 
	Vector3 sBasis1(_CurrentScale, 0.f, 0.f);
	Vector3 sBasis2(0.f, _CurrentScale, 0.f);
	Vector3 sBasis3 = Vector3::UnitZ;
	Matrix3x3 sMat(sBasis1, sBasis2, sBasis3);

	// ���� ��ȯ ��� ( ȸ�� ) 
	float sin, cos;
	Math::GetSinCos(sin, cos, _CurrentDegree);
	Vector3 rBasis1(cos, sin, 0.f);
	Vector3 rBasis2(-sin, cos, 0.f);
	Vector3 rBasis3 = Vector3::UnitZ;
	Matrix3x3 rMat(rBasis1, rBasis2, rBasis3);

	// ���� ��ȯ ��� ( �̵� ) 
	Vector3 tBasis1 = Vector3::UnitX;
	Vector3 tBasis2 = Vector3::UnitY;
	Vector3 tBasis3(_CurrentPosition.X, _CurrentPosition.Y, 1.f);
	Matrix3x3 tMat(tBasis1, tBasis2, tBasis3);

	// ��� ���� ��ȯ�� ���� ���
	Matrix3x3 cMat = tMat * rMat * sMat;

	// ������ ����� ����
	for (int vi = 0; vi < vertexCount; ++vi)
	{
		vertices[vi].Position = cMat * vertices[vi].Position;
	}

	for (int ti = 0; ti < triangleCount; ++ti)
	{
		// �ﰢ������ ĥ�ϱ�
		int bi = ti * 3;
		std::vector<Vertex> tv = { vertices[indices[bi]] , vertices[indices[bi + 1]], vertices[indices[bi + 2]] };

		Vector2 minPos(Math::Min3(tv[0].Position.X, tv[1].Position.X, tv[2].Position.X), Math::Min3(tv[0].Position.Y, tv[1].Position.Y, tv[2].Position.Y));
		Vector2 maxPos(Math::Max3(tv[0].Position.X, tv[1].Position.X, tv[2].Position.X), Math::Max3(tv[0].Position.Y, tv[1].Position.Y, tv[2].Position.Y));

		// �����߽���ǥ�� ���� �غ��۾�
		Vector2 u = tv[1].Position - tv[0].Position;
		Vector2 v = tv[2].Position - tv[0].Position;

		// ���� �и� ���� ���� ��. ( uu * vv - uv * uv )
		float udotv = u.Dot(v);
		float vdotv = v.Dot(v);
		float udotu = u.Dot(u);
		float denominator = udotv * udotv - vdotv * udotu;
		if (Math::EqualsInTolerance(denominator, 0.0f))
		{
			continue;
		}
		float invDenominator = 1.f / denominator;

		// ȭ����� �� ���ϱ�
		ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
		ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

		// ��� ���� Loop
		for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
		{
			for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
			{
				ScreenPoint fragment = ScreenPoint(x, y);
				Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
				Vector2 w = pointToTest - tv[0].Position;
				float wdotu = w.Dot(u);
				float wdotv = w.Dot(v);

				float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
				float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
				float oneMinusST = 1.f - s - t;
				if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
				{
					Vector2 outUV = tv[0].UV * oneMinusST + tv[1].UV * s + tv[2].UV * t;
					_RSI->DrawPoint(fragment, _GameEngine.GetMainTexture().GetColor(outUV));
				}
			}
		}
	}

	// ���� ������ ȭ�� ���
	_RSI->PushStatisticText(std::string("Position : ") + _CurrentPosition.ToString());
	_RSI->PushStatisticText(std::string("Rotation : ") + std::to_string(_CurrentDegree));
	_RSI->PushStatisticText(std::string("Scale : ") + std::to_string(_CurrentScale));
}

