
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

	Vertex3D operator*(float InScalar) const
	{
		return Vertex3D(
			Position * InScalar,
			Color * InScalar,
			UV * InScalar
		);
	}

	Vertex3D operator+(const Vertex3D& InVector) const
	{
		return Vertex3D(
			Position + InVector.Position,
			Color + InVector.Color,
			UV + InVector.UV
		);
	}

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
	_PerspMatrix = perspMat;
	Matrix4x4 pvMat = perspMat * viewMat;
	ScreenPoint viewportSize = mainCamera.GetViewportSize();
	float nearZ = mainCamera.GetNearZ();
	float farZ = mainCamera.GetFarZ();

	size_t totalObjects = _GameEngine3.GetScene().size();
	size_t culledObjects = 0;
	size_t renderedObjects = 0;

	// ����ü �ø��� ���� �غ� �۾�. �� ���͸� ���� ���� �� �ְ� ��ġ���� �д�.
	Matrix4x4 perspMatT = perspMat.Tranpose();
	std::vector<Vector4> frustumVectors = {
		perspMatT[3] + perspMatT[0],
		perspMatT[3] - perspMatT[0],
		perspMatT[3] + perspMatT[1],
		perspMatT[3] - perspMatT[1],
		perspMatT[3] + perspMatT[2],
		perspMatT[3] + perspMatT[2]
	};

	const Texture& mainTexture = _GameEngine3.GetMainTexture();

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

			//float k = perspMat[2][2];
			//float l = perspMat[3][2];
			//float f = l / (k + 1.f);
			//float n = l / (k - 1.f);

			//Vector4 v0 = tv0.Position;
			//Vector4 v1 = tv1.Position;
			//Vector4 v2 = tv2.Position;

			//if (v0.W == 0.f) v0.W = KINDA_SMALL_NUMBER;
			//if (v1.W == 0.f) v1.W = KINDA_SMALL_NUMBER;
			//if (v2.W == 0.f) v2.W = KINDA_SMALL_NUMBER;

			//v0 /= v0.W;
			//v1 /= v1.W;
			//v2 /= v2.W;

			//v0.X *= (_ScreenSize.X * 0.5f);
			//v0.Y *= (_ScreenSize.Y * 0.5f);
			//v1.X *= (_ScreenSize.X * 0.5f);
			//v1.Y *= (_ScreenSize.Y * 0.5f);
			//v2.X *= (_ScreenSize.X * 0.5f);
			//v2.Y *= (_ScreenSize.Y * 0.5f);

			//Vector3 edge1 = (v1 - v0).ToVector3();
			//Vector3 edge2 = (v2 - v0).ToVector3();
			//Vector3 cross = edge1.Cross(edge2);


			//Matrix4x4 invPMat(Vector4::UnitX * invPx, Vector4::UnitY * invPy, Vector4::UnitW * invL, Vector4(0.f, 0.f, -1.f, k * invL));
			//

			// �����̽� �ø� ( �޸��̸� �׸��� ���� )
			// (x2y3-x3y2)/w2w3 + (x1y2-x2y1)/w1w2 + (x3y1-x1y3)/w1w3




			//float p1 = (tv1.Position.X * tv2.Position.Y - tv1.Position.Y * tv2.Position.X);
			//float p2 = (tv0.Position.X * tv1.Position.Y - tv0.Position.Y * tv1.Position.X);
			//float p3 = (tv2.Position.X * tv0.Position.Y - tv2.Position.Y * tv0.Position.X);
			//float d1 = tv1.Position.W * tv2.Position.W;
			//float d2 = tv0.Position.W * tv1.Position.W;
			//float d3 = tv0.Position.W * tv2.Position.W;
			//if (d1 == 0.f) d1 = KINDA_SMALL_NUMBER;
			//if (d2 == 0.f) d2 = KINDA_SMALL_NUMBER;
			//if (d3 == 0.f) d3 = KINDA_SMALL_NUMBER;
			//float z = p1 / d1 + p2 / d2 + p3 / d3;
			//if (z <= 0.f)
			//{
			//	continue;
			//}

			//Vector3 edge1 = (vertices[1] - vertices[0]).ToVector3();
			//Vector3 edge2 = (vertices[2] - vertices[0]).ToVector3();
			//if (edge1.Cross(edge2).Z > 0.f)
			//{
			//	return;
			//}



			bool tv0UnderW0 = false;
			bool tv1UnderW0 = false;
			bool tv2UnderW0 = false;
			UINT16 nCountUnderW0 = 0;
			if (tv0.Position.W < 0.f)
			{
				nCountUnderW0++;
				tv0UnderW0 = true;
			}
			if (tv1.Position.W < 0.f)
			{
				nCountUnderW0++;
				tv1UnderW0 = true;
			}
			if (tv2.Position.W < 0.f)
			{
				nCountUnderW0++;
				tv2UnderW0 = true;
			}

			if (nCountUnderW0 == 0) // �״�� �׸���
			{
				std::vector<Vertex3D> newVertices;
				newVertices.push_back(tv0);
				newVertices.push_back(tv1);
				newVertices.push_back(tv2);
				DrawTriangle(newVertices, mainTexture);
			}
			else if (nCountUnderW0 == 1) // Ŭ�����ϱ�
			{
				Vertex3D underW0Pos;
				Vertex3D nonClippedPos1;
				Vertex3D nonClippedPos2;
				if (tv0UnderW0) // edge0�� edge1�� Ŭ�����ϱ�
				{
					underW0Pos = tv0;
					nonClippedPos1 = tv1;
					nonClippedPos2 = tv2;
				}
				else if (tv1UnderW0) // edge0�� edge2�� Ŭ�����ϱ�
				{
					underW0Pos = tv1;
					nonClippedPos1 = tv2;
					nonClippedPos2 = tv0;
				}
				else if (tv2UnderW0) // edge1�� edge2�� Ŭ�����ϱ�
				{
					underW0Pos = tv2;
					nonClippedPos1 = tv0;
					nonClippedPos2 = tv1;
				}

				// W=0���� Ŭ�����ϱ�
				float t1 = underW0Pos.Position.W / (underW0Pos.Position.W - nonClippedPos1.Position.W);
				float t2 = underW0Pos.Position.W / (underW0Pos.Position.W - nonClippedPos2.Position.W);
				Vertex3D clippedPos1 = underW0Pos * (1.f - t1) + nonClippedPos1 * t1;
				Vertex3D clippedPos2 = underW0Pos * (1.f - t2) + nonClippedPos2 * t2;

				// �������� Ŭ�����ϱ�
				float p11 = clippedPos1.Position.W + clippedPos1.Position.Z;
				float p12 = nonClippedPos1.Position.W + nonClippedPos1.Position.Z;
				float p21 = clippedPos2.Position.W + clippedPos2.Position.Z;
				float p22 = nonClippedPos2.Position.W + nonClippedPos2.Position.Z;
				t1 = p11 / (p11 - p12);
				t2 = p21 / (p21 - p22);
				clippedPos1 = clippedPos1 * (1.f - t1) + nonClippedPos1 * t1;
				clippedPos2 = clippedPos2 * (1.f - t2) + nonClippedPos2 * t2;

				std::vector<Vertex3D> newVertices;
				newVertices.push_back(nonClippedPos1);
				newVertices.push_back(nonClippedPos2);
				newVertices.push_back(clippedPos1);
				DrawTriangle(newVertices, mainTexture);

				newVertices.clear();
				newVertices.push_back(nonClippedPos2);
				newVertices.push_back(clippedPos2);
				newVertices.push_back(clippedPos1);
				DrawTriangle(newVertices, mainTexture);
			}
			else if (nCountUnderW0 == 2) // Ŭ�����ϱ�
			{
				Vertex3D underW0Pos1;
				Vertex3D underW0Pos2;
				Vertex3D nonClippedPos;
				if (!tv0UnderW0) // edge0�� edge1�� Ŭ�����ϱ�
				{
					nonClippedPos = tv0;
					underW0Pos1 = tv1;
					underW0Pos2 = tv2;
				}
				else if (!tv1UnderW0) // edge0�� edge2�� Ŭ�����ϱ�
				{
					nonClippedPos = tv1;
					underW0Pos1 = tv2;
					underW0Pos2 = tv0;
				}
				else if (!tv2UnderW0) // edge1�� edge2�� Ŭ�����ϱ�
				{
					nonClippedPos = tv2;
					underW0Pos1 = tv0;
					underW0Pos2 = tv1;
				}

				// W=0���� Ŭ�����ϱ�
				float t1 = underW0Pos1.Position.W / (underW0Pos1.Position.W - nonClippedPos.Position.W);
				float t2 = underW0Pos2.Position.W / (underW0Pos2.Position.W - nonClippedPos.Position.W);
				Vertex3D clippedPos1 = underW0Pos1 * (1.f - t1) + nonClippedPos * t1;
				Vertex3D clippedPos2 = underW0Pos2 * (1.f - t2) + nonClippedPos * t2;

				// W=0���� �������� Ŭ�����ϱ�
				float p11 = clippedPos1.Position.W + clippedPos1.Position.Z;
				float p12 = nonClippedPos.Position.W + nonClippedPos.Position.Z;
				float p21 = clippedPos2.Position.W + clippedPos2.Position.Z;
				float p22 = nonClippedPos.Position.W + nonClippedPos.Position.Z;
				t1 = p11 / (p11 - p12);
				t2 = p21 / (p21 - p22);
				clippedPos1 = clippedPos1 * (1.f - t1) + nonClippedPos * t1;
				clippedPos2 = clippedPos2 * (1.f - t2) + nonClippedPos * t2;

				std::vector<Vertex3D> newVertices;
				newVertices.push_back(nonClippedPos);
				newVertices.push_back(clippedPos1);
				newVertices.push_back(clippedPos2);
				DrawTriangle(newVertices, mainTexture);
			}
		}

		renderedObjects++;
	}

	//_RSI->PushStatisticText("Total GameObjects : " + std::to_string(totalObjects));
	//_RSI->PushStatisticText("Culled GameObjects : " + std::to_string(culledObjects));
	//_RSI->PushStatisticText("Rendered GameObjects : " + std::to_string(renderedObjects));
}

void SoftRenderer::DrawTriangle(std::vector<Vertex3D>& vertices, const Texture& InTexture)
{
	for (auto& v : vertices)
	{
		// ���� ������ ���, �ణ �������ش�.
		if (v.Position.W == 0.f) v.Position.W = SMALL_NUMBER;

		float invW = 1.f / v.Position.W;
		v.Position.X *= invW;
		v.Position.Y *= invW;
		v.Position.Z *= invW;

		// ȭ�� �������� Ȯ��
		v.Position.X *= (_ScreenSize.X * 0.5f);
		v.Position.Y *= (_ScreenSize.Y * 0.5f);
	}

	// �����̽� �ø� ( �޸��̸� �׸��� ���� )
	Vector3 edge1 = (vertices[1].Position - vertices[0].Position).ToVector3();
	Vector3 edge2 = (vertices[2].Position - vertices[0].Position).ToVector3();
	float z = edge1.Cross(edge2).Z;
	if (z <= 0.f)
	{
		return;
	}

	// �ﰢ�� ĥ�ϱ�
	// �ﰢ���� ���� ����
	Vector2 minPos(Math::Min3(vertices[0].Position.X, vertices[1].Position.X, vertices[2].Position.X), Math::Min3(vertices[0].Position.Y, vertices[1].Position.Y, vertices[2].Position.Y));
	Vector2 maxPos(Math::Max3(vertices[0].Position.X, vertices[1].Position.X, vertices[2].Position.X), Math::Max3(vertices[0].Position.Y, vertices[1].Position.Y, vertices[2].Position.Y));

	// �����߽���ǥ�� ���� ���� ���ͷ� ��ȯ
	Vector2 u = vertices[1].Position.ToVector2() - vertices[0].Position.ToVector2();
	Vector2 v = vertices[2].Position.ToVector2() - vertices[0].Position.ToVector2();

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
			Vector2 w = pointToTest - vertices[0].Position.ToVector2();
			float wdotu = w.Dot(u);
			float wdotv = w.Dot(v);

			float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
			float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
			float oneMinusST = 1.f - s - t;
			if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
			{
				// �� ������ ������ �� ������ z��
				float invZ0 = 1.f / vertices[0].Position.W;
				float invZ1 = 1.f / vertices[1].Position.W;
				float invZ2 = 1.f / vertices[2].Position.W;

				// ���� ���������� ����� ���� �и�
				float z = invZ0 * oneMinusST + invZ1 * s + invZ2 * t;
				float invZ = 1.f / z;

				// ���� ��꿡 ����� �� ( ���������̹Ƿ� ��->���� ������ �迭�� ���� )
				float k = _PerspMatrix[2][2];
				float l = _PerspMatrix[3][2];
				float f = l / (k + 1.f);
				float n = l / (k - 1.f);

				//float newDepth = invZ * k + l;
				//float newDepth = (invZ - n) / (f - n);
				float newDepth = (vertices[0].Position.Z * oneMinusST * invZ0 + vertices[1].Position.Z * s * invZ1 + vertices[2].Position.Z * t * invZ2) * invZ;
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
					float grayScale = (invZ - n) / (f - n);

					// ���� ���� �׸���
					_RSI->DrawPoint(fragment, LinearColor::White * grayScale);
				}
				else
				{
					// ���������������� ������ �ش� �ȼ��� UV ��
					Vector2 targetUV = (vertices[0].UV * oneMinusST * invZ0 + vertices[1].UV * s * invZ1 + vertices[2].UV * t * invZ2) * invZ;

					// �ؽ��� ���� ����
					_RSI->DrawPoint(fragment, FragmentShader3D(InTexture.GetSample(targetUV)));
				}
			}
		}
	}
}

