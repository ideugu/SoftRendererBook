
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
	Matrix4x4 pMatrix;

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

	// ���� �ð�ȭ ���
	if (input.SpacePressed()) { _ShowDepthBuffer = !_ShowDepthBuffer; }
}

// ������ ����
void SoftRenderer::Render3D()
{
	// ����� �׸���
	DrawGizmo3D();

	const Camera& mainCamera = _GameEngine3.GetMainCamera();
	Matrix4x4 viewMat = mainCamera.GetViewMatrix();
	Matrix4x4 perspMat = mainCamera.GetPerspectiveMatrix();
	Matrix4x4 pvMat = perspMat * viewMat;
	ScreenPoint viewportSize = mainCamera.GetViewportSize();
	float nearZ = mainCamera.GetNearZ();
	float farZ = mainCamera.GetFarZ();

	size_t totalObjects = _GameEngine3.GetScene().size();
	size_t culledObjects = 0;
	size_t renderedObjects = 0;

	// ����ü �ø��� ���� �غ� �۾�. �� ���͸� ���� ���� �� �ְ� ��ġ���ѵд�.
	Matrix4x4 perspMatT = perspMat.Tranpose();
	std::vector<Vector4> frustumVectors = {
		perspMatT[3] + perspMatT[0],
		perspMatT[3] - perspMatT[0],
		perspMatT[3] + perspMatT[1],
		perspMatT[3] - perspMatT[1],
		perspMatT[3] + perspMatT[2],
		perspMatT[3] + perspMatT[2]
	};

	for (auto it = _GameEngine3.SceneBegin(); it != _GameEngine3.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *it;
		const Transform& transform = gameObject.GetTransformConst();

		// ������ǥ�踦 ����� ����ü �ø��� ����
		Vector4 viewPos = viewMat * Vector4(transform.GetPosition());

		bool isOutside = false;
		for(const auto& v : frustumVectors)
		{
			if (v.Dot(viewPos) < 0.f)
			{
				isOutside = true;
				break;
			}
		}

		if (isOutside)
		{
			culledObjects++;
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

		// �ﰢ�� ���� �׸���
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
			Vertex3D& tv0 = vertices[indice[bi0]];
			Vertex3D& tv1 = vertices[indice[bi1]];
			Vertex3D& tv2 = vertices[indice[bi2]];

			// �����̽� �ø� ( �޸��̸� �׸��� ���� )
			Vector3 edge1 = (tv1.Position - tv0.Position).ToVector3();
			Vector3 edge2 = (tv2.Position - tv0.Position).ToVector3();
			if (edge1.Cross(edge2).Z <= 0.f)
			{
				continue;
			}

			UINT16 clipCount = 0;
			// �� ���ο� ���� �׽�Ʈ ����
			Vector4 clip1 = tv0.Position;
			Vector4 clip2 = tv1.Position;
			bool edge0Clipped = ClipLine(clip1, clip2);

			Vector4 clip3 = tv0.Position;
			Vector4 clip4 = tv2.Position;
			bool edge1Clipped = ClipLine(clip3, clip4);

			Vector4 clip5 = tv1.Position;
			Vector4 clip6 = tv2.Position;
			bool edge2Clipped = ClipLine(clip5, clip6);

			std::vector<Vector4> newVertices = {
				tv0.Position, 
				tv1.Position,
				tv2.Position
			};

			if (edge0Clipped)
			{
				clipCount++;
				if (!clip1.EqualsInTolerance(tv0.Position))
				{
					newVertices.push_back(clip1);
				}
				else
				{
					newVertices.push_back(clip2);
				}
			}

			if (edge1Clipped)
			{
				clipCount++;
				if (!clip3.EqualsInTolerance(tv0.Position))
				{
					newVertices.push_back(clip3);
				}
				else
				{
					newVertices.push_back(clip4);
				}
			}

			if (edge2Clipped)
			{
				clipCount++;
				if (!clip5.EqualsInTolerance(tv1.Position))
				{
					newVertices.push_back(clip5);
				}
				else
				{
					newVertices.push_back(clip6);
				}
			}

			if (clipCount > 2)
			{
				// �̷��� ��Ȳ�� �߻��ؼ��� �ȵ�.
			}
			else
			{
				if (newVertices.size() == 3)
				{
					
				}
				else if (newVertices.size() == 4)
				{
					// �ﰢ�� �ѷ� �ɰ���
				}
			}

			pMatrix = perspMat;
			DrawTriangle(tv0, tv1, tv2, newVertices, true, _GameEngine3.GetMainTexture());

		}

		renderedObjects++;
	}

	//_RSI->PushStatisticText("Total GameObjects : " + std::to_string(totalObjects));
	//_RSI->PushStatisticText("Culled GameObjects : " + std::to_string(culledObjects));
	//_RSI->PushStatisticText("Rendered GameObjects : " + std::to_string(renderedObjects));
}

void SoftRenderer::DrawTriangle(Vertex3D tv0, Vertex3D tv1, Vertex3D tv2, const std::vector<Vector4>& vertices, bool DrawTexture, const Texture& InTexture)
{

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
		return;
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
				// �� ������ ������ �� ������ z��
				float invZ0 = 1.f / tv0.Position.W;
				float invZ1 = 1.f / tv1.Position.W;
				float invZ2 = 1.f / tv2.Position.W;

				// ���� ���������� ����� ���� �и�
				float z = invZ0 * oneMinusST + invZ1 * s + invZ2 * t;
				float invZ = 1.f / z;

				// ���� ��꿡 ����� �� ( ���������̹Ƿ� ��->���� ������ �迭�� ���� )
				float k = pMatrix[2][2];
				float l = pMatrix[3][2];
				float newDepth = invZ * k + l;
				//float newDepth = Math::Clamp(-perspMat[2][2] + perspMat[3][2] * z, 0.f, 1.f);
				float prevDepth = _RSI->GetDepthBufferValue(fragment);
				if (newDepth < prevDepth)
				{
					_RSI->SetDepthBufferValue(fragment, newDepth);
				}
				else
				{
					// �̹� �տ� ���� �׷��������Ƿ� �ȼ��׸���� ����
					continue;
				}

				if (_ShowDepthBuffer)
				{
					// �ð�ȭ�� ���� ����ȭ�� ��� ��
					// f = l / (k+1)
					// n = l / (k-1)
					float f = l / (k + 1.f);
					float n = l / (k - 1.f);
					float grayScale = (invZ - n) / (f - n);

					// ���� ���� �׸���
					_RSI->DrawPoint(fragment, LinearColor::White * grayScale);
				}
				else
				{
					if (DrawTexture)
					{
						// ���������������� ������ �ش� �ȼ��� UV ��
						Vector2 targetUV = (tv0.UV * oneMinusST * invZ0 + tv1.UV * s * invZ1 + tv2.UV * t * invZ2) * invZ;

						// �ؽ��� ���� ����
						_RSI->DrawPoint(fragment, FragmentShader3D(InTexture.GetSample(targetUV)));
					}
					else
					{
						_RSI->DrawPoint(fragment, LinearColor::Blue);
					}
				}
			}
		}
	}
}

bool SoftRenderer::ClipLine(CK::Vector4& clip1, CK::Vector4& clip2)
{
	UINT16 outCount = 0;
	UINT16 clipCount = 0;

	// w = 0�� ��鿡 �����ϴ� ���� ������
	if (clip1.W == clip2.W) // ���� ��鿡 ������ ���. 
	{
		if (clip1.W < 0)
		{
			// �ٱ��� ����.
			outCount++;
		}
		else
		{
			// ���ʿ� ����.
		}
	}
	else
	{
		float t = clip1.W / (clip1.W - clip2.W);
		if (t > 0.f && t < 1.f) // �ϳ��� �ۿ� ���� Ŭ������ �ʿ���.
		{
			Vector4 clippedPos = clip1 * (1.f - t) + clip2 * t;
			if (clip1.W < 0.f)
			{
				clip1 = clippedPos;
			}
			else
			{
				clip2 = clippedPos;
			}

			clipCount++;
		}
		else if ((t < 0.f && clip1.W < 0) || (t > 1.f && clip2.W < 0)) // �� �� ��� �ٱ��� ����. Ŭ���� �ʿ� ����.
		{
			outCount++;
		}
		else // �� �� ��� ���ʿ� ����. Ŭ���� �ʿ� ����.
		{

		}
	}

	// w = y�� �����ϴ� ��� ���
	float d1 = clip1.W - clip1.Y;
	float d2 = clip2.W - clip2.Y;
	if (d1 == d2) // ��� ��鿡 ������ ���
	{
		if (clip1.Y > clip1.W)
		{
			// �ٱ��� ����.
			outCount++;
		}
		else
		{
			// ���ʿ� ����.
		}
	}
	else
	{
		float t = d1 / (d1 - d2);
		if (t > 0.f && t < 1.f) // �ϳ��� �ۿ� ���� Ŭ������ �ʿ���.
		{
			Vector4 clippedPos = clip1 * (1.f - t) + clip2 * t;
			if (clip1.Y > clip1.W)
			{
				clip1 = clippedPos;
			}
			else
			{
				clip2 = clippedPos;
			}
			clipCount++;
		}
		else if ((t < 0.f && clip1.Y > clip1.W) || (t > 1.f && clip2.Y > clip2.W)) // �� �� ��� �ٱ��� ����. Ŭ���� �ʿ� ����.
		{
			outCount++;
		}
		else // �� �� ��� ���ʿ� ����. Ŭ���� �ʿ� ����.
		{

		}
	}

	// w = -y�� �����ϴ� �ϴ� ���
	d1 = clip1.W + clip1.Y;
	d2 = clip2.W + clip2.Y;
	if (d1 == d2) // ��� ��鿡 ������ ���
	{
		if (clip1.Y < clip1.W)
		{
			// �ٱ��� ����.
			outCount++;
		}
		else
		{
			// ���ʿ� ����.
		}
	}
	else
	{
		float t = d1 / (d1 - d2);
		if (t > 0.f && t < 1.f) // �ϳ��� �ۿ� ���� Ŭ������ �ʿ���.
		{
			Vector4 clippedPos = clip1 * (1.f - t) + clip2 * t;
			if (clip1.Y < clip1.W)
			{
				clip1 = clippedPos;
			}
			else
			{
				clip2 = clippedPos;
			}
			clipCount++;
		}
		else if ((t < 0.f && clip1.Y < clip1.W) || (t > 1.f && clip2.Y < clip2.W)) // �� �� ��� �ٱ��� ����. Ŭ���� �ʿ� ����.
		{
			outCount++;
		}
		else // �� �� ��� ���ʿ� ����. Ŭ���� �ʿ� ����.
		{

		}
	}

	// w = x�� �����ϴ� ���� ���
	d1 = clip1.W - clip1.X;
	d2 = clip2.W - clip2.X;
	if (d1 == d2) // ��鿡 ������ ���
	{
		if (clip1.X > clip1.W)
		{
			// �ٱ��� ����.
			outCount++;
		}
		else
		{
			// ���ʿ� ����.
		}
	}
	else
	{
		float t = d1 / (d1 - d2);
		if (t > 0.f && t < 1.f) // �ϳ��� �ۿ� ���� Ŭ������ �ʿ���.
		{
			Vector4 clippedPos = clip1 * (1.f - t) + clip2 * t;
			if (clip1.X > clip1.W)
			{
				clip1 = clippedPos;
			}
			else
			{
				clip2 = clippedPos;
			}
			clipCount++;
		}
		else if ((t < 0.f && clip1.X > clip1.W) || (t > 1.f && clip2.X > clip2.W)) // �� �� ��� �ٱ��� ����. Ŭ���� �ʿ� ����.
		{
			outCount++;
		}
		else // �� �� ��� ���ʿ� ����. Ŭ���� �ʿ� ����.
		{

		}
	}

	// w = -x�� �����ϴ� �ϴ� ���
	d1 = clip1.W + clip1.X;
	d2 = clip2.W + clip2.X;
	if (d1 == d2) // ��� ��鿡 ������ ���
	{
		if (clip1.X < clip1.W)
		{
			// �ٱ��� ����.
			outCount++;
		}
		else
		{
			// ���ʿ� ����.
		}
	}
	else
	{
		float t = d1 / (d1 - d2);
		if (t > 0.f && t < 1.f) // �ϳ��� �ۿ� ���� Ŭ������ �ʿ���.
		{
			Vector4 clippedPos = clip1 * (1.f - t) + clip2 * t;
			if (clip1.X < clip1.W)
			{
				clip1 = clippedPos;
			}
			else
			{
				clip2 = clippedPos;
			}
			clipCount++;
		}
		else if ((t < 0.f && clip1.X < clip1.W) || (t > 1.f && clip2.X < clip2.W)) // �� �� ��� �ٱ��� ����. Ŭ���� �ʿ� ����.
		{
			outCount++;
		}
		else // �� �� ��� ���ʿ� ����. Ŭ���� �ʿ� ����.
		{

		}
	}

	// w = -z �� �ش��ϴ� ���
	d1 = clip1.W - clip1.Z;
	d2 = clip2.W - clip2.Z;
	if (d1 == d2) // ��鿡 ������ ���
	{
		if (clip1.Z < clip1.W)
		{
			// �ٱ��� ����.
			outCount++;
		}
		else
		{
			// ���ʿ� ����.
		}
	}
	else
	{
		float t = d1 / (d1 - d2);
		if (t > 0.f && t < 1.f) // �ϳ��� �ۿ� ���� Ŭ������ �ʿ���.
		{
			Vector4 clippedPos = clip1 * (1.f - t) + clip2 * t;
			if (clip1.Z < clip1.W)
			{
				clip1 = clippedPos;
			}
			else
			{
				clip2 = clippedPos;
			}
			clipCount++;
		}
		else if ((t < 0.f && clip1.Z < clip1.W) || (t > 1.f && clip2.X < clip2.W)) // �� �� ��� �ٱ��� ����. Ŭ���� �ʿ� ����.
		{
			outCount++;
		}
		else // �� �� ��� ���ʿ� ����. Ŭ���� �ʿ� ����.
		{

		}
	}

	// w = z�� �����ϴ� ���
	d1 = clip1.W + clip1.Z;
	d2 = clip2.W + clip2.Z;
	if (d1 == d2) // ��� ��鿡 ������ ���
	{
		if (clip1.Z > clip1.W)
		{
			// �ٱ��� ����.
			outCount++;
		}
		else
		{
			// ���ʿ� ����.
		}
	}
	else
	{
		float t = d1 / (d1 - d2);
		if (t > 0.f && t < 1.f) // �ϳ��� �ۿ� ���� Ŭ������ �ʿ���.
		{
			Vector4 clippedPos = clip1 * (1.f - t) + clip2 * t;
			if (clip1.Z > clip1.W)
			{
				clip1 = clippedPos;
			}
			else
			{
				clip2 = clippedPos;
			}
			clipCount++;
		}
		else if ((t < 0.f && clip1.Z > clip1.W) || (t > 1.f && clip2.Z > clip2.W)) // �� �� ��� �ٱ��� ����. Ŭ���� �ʿ� ����.
		{
			outCount++;
		}
		else // �� �� ��� ���ʿ� ����. Ŭ���� �ʿ� ����.
		{

		}
	}

	return clipCount > 0 ? true : false;
}

