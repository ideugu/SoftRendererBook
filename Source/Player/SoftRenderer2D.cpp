
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
		GetRSI().DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy)
	{
		GetRSI().DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	// ������ ����
	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	GetRSI().DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	GetRSI().DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// ���� ����
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	static float elapsedTime = 0.f;
	static float moveSpeed = 200.f;
	static float rotateSpeed = 180.f;
	static float scaleSpeed = 180.f;

	InputManager& input = _GameEngine2.GetInputManager();

	// �÷��̾� ���� ������Ʈ Ʈ�������� ����
	GameObject& player = _GameEngine2.FindGameObject(GameEngine::PlayerKey);
	if (!player.Found())
	{
		Transform& playerTransform = player.GetTransform();
		playerTransform.AddPosition(Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)) * moveSpeed * InDeltaSeconds);
		playerTransform.AddRotation(input.GetAxis(InputAxis::WAxis) * rotateSpeed * InDeltaSeconds);
		float newScale = Math::Clamp(playerTransform.GetScale().X + scaleSpeed * input.GetAxis(InputAxis::ZAxis) * InDeltaSeconds, 15.f, 30.f);
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

	elapsedTime += InDeltaSeconds;
	elapsedTime = Math::FMod(elapsedTime, 1.f);
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

		// �������� ����� ���� ���ۿ� �ε��� ���۷� ��ȯ
		std::vector<Vertex2D> vertices(vertexCount);
		std::vector<int> indice(mesh._Indices);
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi].Position = mesh._Vertices[vi];
			vertices[vi].UV = mesh._UVs[vi];
		}

		// ���� ��ȯ ����
		VertexShader2D(vertices, finalMat);

		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
			Vertex2D& tv0 = vertices[indice[bi0]];
			Vertex2D& tv1 = vertices[indice[bi1]];
			Vertex2D& tv2 = vertices[indice[bi2]];

			if (gameObject != GameEngine::PlayerKey)
			{
				// �÷��̾ �ƴϸ� ���̾����������� ������
				GetRSI().DrawLine(tv0.Position, tv1.Position, gameObject.GetColor());
				GetRSI().DrawLine(tv0.Position, tv2.Position, gameObject.GetColor());
				GetRSI().DrawLine(tv1.Position, tv2.Position, gameObject.GetColor());
			}
			else
			{
				// �÷��̾�� �ؽ��Ŀ� ������ ������ ������
				Vector2 minPos(Math::Min3(tv0.Position.X, tv1.Position.X, tv2.Position.X), Math::Min3(tv0.Position.Y, tv1.Position.Y, tv2.Position.Y));
				Vector2 maxPos(Math::Max3(tv0.Position.X, tv1.Position.X, tv2.Position.X), Math::Max3(tv0.Position.Y, tv1.Position.Y, tv2.Position.Y));

				// �����߽���ǥ�� ���� ���� ���ͷ� ��ȯ
				Vector2 u = tv1.Position - tv0.Position;
				Vector2 v = tv2.Position - tv0.Position;

				// ���� �и� �� ( uu * vv - uv * uv )
				float udotv = u.Dot(v);
				float vdotv = v.Dot(v);
				float udotu = u.Dot(u);
				float denominator = udotv * udotv - vdotv * udotu;

				// ��ȭ �ﰢ���� ����
				if (Math::EqualsInTolerance(denominator, 0.0f))
				{
					// ��ȭ �ﰢ���̸� �ǳʶ�.
					continue;
				}
				float invDenominator = 1.f / denominator;

				// ȭ����� �� ���ϱ�
				ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
				ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

				// �ﰢ�� ���� �� ��� ���� �����ϰ� ��ĥ
				for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
				{
					for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
					{
						ScreenPoint fragment = ScreenPoint(x, y);
						Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
						Vector2 w = pointToTest - tv0.Position;
						float wdotu = w.Dot(u);
						float wdotv = w.Dot(v);

						float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
						float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
						float oneMinusST = 1.f - s - t;
						if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
						{
							Vector2 targetUV = tv0.UV * oneMinusST + tv1.UV * s + tv2.UV * t;
							GetRSI().DrawPoint(fragment, FragmentShader2D(_GameEngine2.GetMainTexture().GetSample(targetUV), LinearColor::White));
						}
					}
				}
			}
		}
	}

	GetRSI().PushStatisticText("Total Game Objects : " + std::to_string(totalObjectCount));

	const GameObject& player = _GameEngine2.FindGameObject(GameEngine::PlayerKey);
	if (!player.Found())
	{
		const Transform& playerTransform = player.GetTransformConst();
		GetRSI().PushStatisticText("Player Position : " + playerTransform.GetPosition().ToString());
		GetRSI().PushStatisticText("Player Rotation : " + std::to_string(playerTransform.GetRotation()) + " (deg)");
		GetRSI().PushStatisticText("Player Scale : " + std::to_string(playerTransform.GetScale().X));
	}
	
}

