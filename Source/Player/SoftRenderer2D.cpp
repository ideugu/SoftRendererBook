
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

struct Vertex2D
{
public:
	Vertex2D() = default;
	Vertex2D(const Vector2& InPosition, const Vector2& InUV) : Position(InPosition), UV(InUV) { }

	Vector2 Position;
	Vector2 UV;
};


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

	// �÷��̾� ���� ������Ʈ Ʈ�������� ����
	GameObject& player = _GameEngine2.FindGameObject(GameEngine::PlayerKey);
	if (!player.IsNotFound())
	{
		Transform& playerTransform = player.GetTransform();
		playerTransform.AddPosition(Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds);
		playerTransform.AddRotation(input.GetWAxis() * rotateSpeed * InDeltaSeconds);
		float newScale = Math::Clamp(playerTransform.GetScale().X + scaleSpeed * input.GetZAxis() * InDeltaSeconds, 15.f, 30.f);
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
		const GameObject& gameObject = *it;
		if (!gameObject.HasMesh())
		{
			continue;
		}

		const Mesh& mesh = _GameEngine2.GetMesh(gameObject.GetMeshKey());
		const Transform& transform = gameObject.GetTransformConst();
		Matrix3x3 finalMat = viewMat * transform.GetModelingMatrix();

		size_t vertexCount = mesh._Vertices.size();
		size_t indexCount = mesh._Indices.size();
		size_t triangleCount = indexCount / 3;

		// �������� ����� ���� ���ۿ� �ε��� ���� ����
		Vertex2D* vertices = new Vertex2D[vertexCount];
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi].Position = mesh._Vertices[vi];
			vertices[vi].UV = mesh._UVs[vi];
		}

		int* indices = new int[indexCount];
		std::memcpy(indices, &mesh._Indices[0], sizeof(int) * indexCount);

		// �� ������ ����� ����
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi].Position = finalMat * vertices[vi].Position;
		}

		if (gameObject != GameEngine::PlayerKey)
		{
			// ��ȯ�� ������ �մ� �� �׸���
			for (int ti = 0; ti < triangleCount; ++ti)
			{
				int bi = ti * 3;
				_RSI->DrawLine(vertices[indices[bi]].Position, vertices[indices[bi + 1]].Position, gameObject.GetColor());
				_RSI->DrawLine(vertices[indices[bi]].Position, vertices[indices[bi + 2]].Position, gameObject.GetColor());
				_RSI->DrawLine(vertices[indices[bi + 1]].Position, vertices[indices[bi + 2]].Position, gameObject.GetColor());
			}
		}
		else
		{
			for (int ti = 0; ti < triangleCount; ++ti)
			{
				// �ﰢ������ ĥ�ϱ�
				int bi = ti * 3;
				std::vector<Vertex2D> tv = { vertices[indices[bi]] , vertices[indices[bi + 1]], vertices[indices[bi + 2]] };

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
							_RSI->DrawPoint(fragment, _GameEngine2.GetMainTexture().GetColor(outUV));
						}
					}
				}
			}
		}

		delete[] vertices;
		delete[] indices;
	}

	_RSI->PushStatisticText("Total Game Objects : " + std::to_string(totalObjectCount));

	const GameObject& player = _GameEngine2.FindGameObject(GameEngine::PlayerKey);
	if (!player.IsNotFound())
	{
		const Transform& playerTransform = player.GetTransformConst();
		_RSI->PushStatisticText("Player Position : " + playerTransform.GetPosition().ToString());
		_RSI->PushStatisticText("Player Rotation : " + std::to_string(playerTransform.GetRotation()) + " (deg)");
		_RSI->PushStatisticText("Player Scale : " + std::to_string(playerTransform.GetScale().X));
	}
	
}

