
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DDD;

struct Vertex3D
{
public:
	Vertex3D() = default;
	Vertex3D(const Vector4& InPosition) : Position(InPosition) { }
	Vertex3D(const Vector4& InPosition, const LinearColor& InColor) : Position(InPosition), Color(InColor) { }
	Vertex3D(const Vector4& InPosition, const LinearColor& InColor, const Vector2& InUV) : Position(InPosition), Color(InColor), UV(InUV) { }

	Vector4 Position;
	LinearColor Color;
	Vector2 UV;
};

namespace CK::DDD
{
	// ���� ��ȯ �ڵ�
	FORCEINLINE void VertexShader3D(std::vector<Vertex3D>& InVertices, Matrix4x4 InMatrix)
	{
		// ��ġ ���� ���� ����� ������ ��ȯ
		for (Vertex3D& v : InVertices)
		{
			v.Position = InMatrix * v.Position;
		}
	}

	LinearColor colorParam;

	// �ȼ� ��ȯ �ڵ�
	FORCEINLINE LinearColor FragmentShader3D(LinearColor InColor)
	{
		return InColor;
	}
}


// �׸��� �׸���
void SoftRenderer::DrawGizmo3D()
{
	// �� ����� �׸���
	std::vector<Vertex3D> viewGizmo = { 
		Vertex3D(Vector4(Vector3::Zero)),
		Vertex3D(Vector4(Vector3::UnitX * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitY * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitZ * _GizmoUnitLength)),
	};

	Matrix4x4 viewMatRotationOnly = _GameEngine3.GetMainCamera().GetViewMatrixRotationOnly();
	VertexShader3D(viewGizmo, viewMatRotationOnly);

	// �� �׸���
	Vector2 v0 = viewGizmo[0].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v1 = viewGizmo[1].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v2 = viewGizmo[2].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v3 = viewGizmo[3].Position.ToVector2() + _GizmoPositionOffset;
	_RSI->DrawLine(v0, v1, LinearColor::Red);
	_RSI->DrawLine(v0, v2, LinearColor::Green);
	_RSI->DrawLine(v0, v3, LinearColor::Blue);
}

// ���� ����
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	static float moveSpeed = 500.f;
	static float fovSpeed = 100.f;

	InputManager input = _GameEngine3.GetInputManager();

	// �÷��̾� ���� ������Ʈ Ʈ�������� ����
	GameObject& player = _GameEngine3.FindGameObject(GameEngine::PlayerKey);
	if (!player.IsNotFound())
	{
		Transform& playerTransform = player.GetTransform();
		playerTransform.AddPosition(Vector3(input.GetXAxis(), input.GetWAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds);
		_GameEngine3.GetMainCamera().SetLookAtRotation(player.GetTransform().GetPosition());
	}

	// ī�޶� FOV ����
	Camera& camera = _GameEngine3.GetMainCamera();
	float newFOV = Math::Clamp(camera.GetFOV() + input.GetZAxis() * fovSpeed * InDeltaSeconds, 5.f, 179.f);
	camera.SetFOV(newFOV);

	// ����� ���
	if (input.SpacePressed()) { _Show3DGizmo = !_Show3DGizmo; }
}

// ������ ����
void SoftRenderer::Render3D()
{
	// ����� �׸���
	if (_Show3DGizmo)
	{
		DrawGizmo3D();
	}

	const Camera& mainCamera = _GameEngine3.GetMainCamera();
	Matrix4x4 viewMat = mainCamera.GetViewMatrix();
	Matrix4x4 perspMat = mainCamera.GetPerspectiveMatrix();
	Matrix4x4 pvMat = perspMat * viewMat;
	ScreenPoint viewportSize = mainCamera.GetViewportSize();
	float nearZ = mainCamera.GetNearZ();

	static float playerDepth = 0.f;
	static float distanceFromCamera = 0.f;

	for (auto it = _GameEngine3.SceneBegin(); it != _GameEngine3.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *it;
		const Transform& transform = gameObject.GetTransformConst();

		// ���� ��ǥ�� ��ȯ
		Vector4 clippedPos = pvMat * Vector4(transform.GetPosition());
		float cameraDepth = clippedPos.W;
		// 0�� ������ ���� ����.
		if (Math::EqualsInTolerance(cameraDepth, 0.f)) { cameraDepth = KINDA_SMALL_NUMBER; }
		float ndcZ = clippedPos.Z / cameraDepth;

		// �ֿ� ��ǥ �� ����
		if (gameObject.GetName() == _GameEngine3.PlayerKey)
		{
			playerDepth = ndcZ;
			distanceFromCamera = clippedPos.W;
		}

		// ���� ������Ʈ�� ��ġ�� �������� ������ ��� �� �׸��� �ʵ��� ó��
		if (ndcZ < -1.f || ndcZ > 1.f)
		{
			continue;
		}

		const Mesh& mesh = _GameEngine3.GetMesh(gameObject.GetMeshKey());
		Matrix4x4 finalMat = pvMat * transform.GetModelingMatrix();

		size_t vertexCount = mesh._Vertices.size();
		size_t indexCount = mesh._Indices.size();
		size_t triangleCount = indexCount / 3;
		bool hasUV = mesh._UVs.size() > 0;

		// �������� ����� ���� ���ۿ� �ε��� ���۷� ��ȯ
		std::vector<Vertex3D> vertices(vertexCount);
		std::vector<int> indice(mesh._Indices);
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi].Position = Vector4(mesh._Vertices[vi]);
			if(hasUV)
			{
				vertices[vi].UV = mesh._UVs[vi];
			}
		}

		// ���� ��ȯ ����
		VertexShader3D(vertices, finalMat);

		// Ŭ�� ������ ȭ�� �������� ��ȯ
		for (Vertex3D& v : vertices)
		{
			// 0�� ������ ���� ����.
			if (Math::EqualsInTolerance(v.Position.W, 0.f)) { v.Position.W = KINDA_SMALL_NUMBER; }

			// NDC �������� ��ȯ
			float invW = 1.f / v.Position.W;
			v.Position.X *= invW;
			v.Position.Y *= invW;
			v.Position.Z *= invW;

			// ȭ�� �������� Ȯ��
			v.Position.X *= (viewportSize.X * 0.5f);
			v.Position.Y *= (viewportSize.Y * 0.5f);
		}

		// ��ȯ�� ������ �մ� �� �׸���
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
			Vertex3D& tv0 = vertices[indice[bi0]];
			Vertex3D& tv1 = vertices[indice[bi1]];
			Vertex3D& tv2 = vertices[indice[bi2]];

			// �� ���̶� ����� �ڿ� �ִٸ� �׸��� �ʵ��� ������ġ ����
			if (tv0.Position.W < nearZ || tv1.Position.W < nearZ || tv2.Position.W < nearZ)
			{
				continue;
			}

			// �����̽� �ø� ( �޸��̸� �׸��� ���� )
			Vector3 edge1 = (tv1.Position - tv0.Position).ToVector3();
			Vector3 edge2 = (tv2.Position - tv0.Position).ToVector3();
			if (edge1.Cross(edge2).Z <= 0.f)
			{
				continue;
			}

			// ���� ������Ʈ�� ���� ����
			LinearColor objectColor = FragmentShader3D(gameObject.GetColor());

			if (gameObject == GameEngine::PlayerKey)
			{
				// ���̾������� �׸���
				_RSI->DrawLine(tv0.Position, tv1.Position, objectColor);
				_RSI->DrawLine(tv0.Position, tv2.Position, objectColor);
				_RSI->DrawLine(tv1.Position, tv2.Position, objectColor);
			}
			else
			{
				if (!_Show3DGizmo)
					continue;

				// �ﰢ�� ĥ�ϱ�
				// �ﰢ���� ���� ����
				Vector2 minPos(Math::Min3(tv0.Position.X, tv1.Position.X, tv2.Position.X), Math::Min3(tv0.Position.Y, tv1.Position.Y, tv2.Position.Y));
				Vector2 maxPos(Math::Max3(tv0.Position.X, tv1.Position.X, tv2.Position.X), Math::Max3(tv0.Position.Y, tv1.Position.Y, tv2.Position.Y));

				// �����߽���ǥ�� ���� ���� ���ͷ� ��ȯ
				Vector2 u = tv1.Position.ToVector2() - tv0.Position.ToVector2();
				Vector2 v = tv2.Position.ToVector2() - tv0.Position.ToVector2();

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

				// �� ���� ȭ�� ���� ����� ��� Ŭ���� ó��
				lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
				lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
				upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
				upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

				// �ﰢ�� ���� �� ��� ���� �����ϰ� ��ĥ
				for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
				{
					for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
					{
						ScreenPoint fragment = ScreenPoint(x, y);
						Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
						Vector2 w = pointToTest - tv0.Position.ToVector2();
						float wdotu = w.Dot(u);
						float wdotv = w.Dot(v);

						float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
						float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
						float oneMinusST = 1.f - s - t;
						if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
						{
							_RSI->DrawPoint(fragment, objectColor);
						}
					}
				}
			}
		}
	}

	_RSI->PushStatisticText("Camera : " + mainCamera.GetTransformConst().GetPosition().ToString());
	_RSI->PushStatisticText("Camera FOV : " + std::to_string(mainCamera.GetFOV()));
	const GameObject& player = _GameEngine3.FindGameObject(GameEngine::PlayerKey);
	if (!player.IsNotFound())
	{
		const Transform& playerTransform = player.GetTransformConst();
		_RSI->PushStatisticText("Player : " + playerTransform.GetPosition().ToString());
		_RSI->PushStatisticText("Player Depth: " + std::to_string(playerDepth));
		_RSI->PushStatisticText("Distance: " + std::to_string(distanceFromCamera));
	}
}

