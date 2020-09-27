
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

			float k = perspMat[2][2];
			float l = perspMat[3][2];
			float f = l / (k + 1.f);
			float n = l / (k - 1.f);

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
				std::vector<Vector4> newVertices;
				newVertices.push_back(tv0.Position);
				newVertices.push_back(tv1.Position);
				newVertices.push_back(tv2.Position);
				DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());
			}
			else if (nCountUnderW0 == 1) // Ŭ�����ϱ�
			{
				Vector4 underW0Pos;
				Vector4 nonClippedPos1;
				Vector4 nonClippedPos2;
				if (tv0UnderW0) // edge0�� edge1�� Ŭ�����ϱ�
				{
					underW0Pos = tv0.Position;
					nonClippedPos1 = tv1.Position;
					nonClippedPos2 = tv2.Position;
				}
				else if (tv1UnderW0) // edge0�� edge2�� Ŭ�����ϱ�
				{
					underW0Pos = tv1.Position;
					nonClippedPos1 = tv2.Position;
					nonClippedPos2 = tv0.Position;
				}
				else if (tv2UnderW0) // edge1�� edge2�� Ŭ�����ϱ�
				{
					underW0Pos = tv2.Position;
					nonClippedPos1 = tv0.Position;
					nonClippedPos2 = tv1.Position;
				}

				// W=0���� Ŭ�����ϱ�
				float t1 = underW0Pos.W / (underW0Pos.W - nonClippedPos1.W);
				float t2 = underW0Pos.W / (underW0Pos.W - nonClippedPos2.W);
				Vector4 clippedPos1 = underW0Pos * (1.f - t1) + nonClippedPos1 * t1;
				Vector4 clippedPos2 = underW0Pos * (1.f - t2) + nonClippedPos2 * t2;

				// �������� Ŭ�����ϱ�
				float p11 = clippedPos1.W + clippedPos1.Z;
				float p12 = nonClippedPos1.W + nonClippedPos1.Z;
				float p21 = clippedPos2.W + clippedPos2.Z;
				float p22 = nonClippedPos2.W + nonClippedPos2.Z;
				t1 = p11 / (p11 - p12);
				t2 = p21 / (p21 - p22);
				clippedPos1 = clippedPos1 * (1.f - t1) + nonClippedPos1 * t1;
				clippedPos2 = clippedPos2 * (1.f - t2) + nonClippedPos2 * t2;

				std::vector<Vector4> newVertices;
				newVertices.push_back(nonClippedPos1);
				newVertices.push_back(nonClippedPos2);
				newVertices.push_back(clippedPos1);
				DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());

				newVertices.clear();
				newVertices.push_back(nonClippedPos2);
				newVertices.push_back(clippedPos2);
				newVertices.push_back(clippedPos1);
				DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());

			}
			else if (nCountUnderW0 == 2) // Ŭ�����ϱ�
			{
				Vector4 underW0Pos1;
				Vector4 underW0Pos2;
				Vector4 nonClippedPos;
				if (!tv0UnderW0) // edge0�� edge1�� Ŭ�����ϱ�
				{
					nonClippedPos = tv0.Position;
					underW0Pos1 = tv1.Position;
					underW0Pos2 = tv2.Position;
				}
				else if (!tv1UnderW0) // edge0�� edge2�� Ŭ�����ϱ�
				{
					nonClippedPos = tv1.Position;
					underW0Pos1 = tv2.Position;
					underW0Pos2 = tv0.Position;
				}
				else if (!tv2UnderW0) // edge1�� edge2�� Ŭ�����ϱ�
				{
					nonClippedPos = tv2.Position;
					underW0Pos1 = tv0.Position;
					underW0Pos2 = tv1.Position;
				}

				// W=0���� Ŭ�����ϱ�
				float t1 = underW0Pos1.W / (underW0Pos1.W - nonClippedPos.W);
				float t2 = underW0Pos2.W / (underW0Pos2.W - nonClippedPos.W);
				Vector4 clippedPos1 = underW0Pos1 * (1.f - t1) + nonClippedPos * t1;
				Vector4 clippedPos2 = underW0Pos2 * (1.f - t2) + nonClippedPos * t2;

				// �������� Ŭ�����ϱ�
				float p11 = clippedPos1.W + clippedPos1.Z;
				float p12 = nonClippedPos.W + nonClippedPos.Z;
				float p21 = clippedPos2.W + clippedPos2.Z;
				float p22 = nonClippedPos.W + nonClippedPos.Z;
				t1 = p11 / (p11 - p12);
				t2 = p21 / (p21 - p22);
				clippedPos1 = clippedPos1 * (1.f - t1) + nonClippedPos * t1;
				clippedPos2 = clippedPos2 * (1.f - t2) + nonClippedPos * t2;

				std::vector<Vector4> newVertices;
				newVertices.push_back(nonClippedPos);
				newVertices.push_back(clippedPos1);
				newVertices.push_back(clippedPos2);
				DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());
			}

			// �� �� ��� ����� �ڿ� ���� ���� �׸� �ʿ䰡 ����.
			//if (tv0.Position.W <= n && tv1.Position.W <= n && tv2.Position.W <= n)
			//{
			//	continue;
			//}

			//bool isClippingRequired = true;
			//// �� �� ��� ����� �տ� ���� ���� Ŭ������ �ʿ� ����.
			//if (tv0.Position.W >= n && tv1.Position.W >= n && tv2.Position.W >= n)
			//{
			//	isClippingRequired = false;
			//}

			//bool isW0ClippingRequired = false;
			//if (tv0.Position.W < 0.f || tv1.Position.W < 0.f || tv2.Position.W < 0.f)
			//{
			//	isW0ClippingRequired = true;
			//}

			//bool isTriangleSplitRequired = false;


			//std::vector<Vector4> newVertices;
			//newVertices.push_back(tv0.Position);
			//newVertices.push_back(tv1.Position);
			//newVertices.push_back(tv2.Position);
			//DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());

			//UINT16 nCount = 0;
			//if ((tv0.Position.W < 0.f) && (tv1.Position.W >= 0.f) && (tv2.Position.W >= 0.f))
			//{
			//	nCount++;
			//}
			//else if ((tv0.Position.W >= 0.f) && (tv1.Position.W < 0.f) && (tv2.Position.W >= 0.f))
			//{
			//	nCount++;
			//}
			//else if ((tv0.Position.W >= 0.f) && (tv1.Position.W >= 0.f) && (tv2.Position.W < 0.f))
			//{
			//	nCount++;
			//}


			//// �� ���ο� ���� �׽�Ʈ ����
			//Vector4 clip1 = tv0.Position;
			//Vector4 clip2 = tv1.Position;
			//bool edge0Clipped = ClipLine(clip1, clip2);

			//Vector4 clip3 = tv0.Position;
			//Vector4 clip4 = tv2.Position;
			//bool edge1Clipped = ClipLine(clip3, clip4);

			//Vector4 clip5 = tv1.Position;
			//Vector4 clip6 = tv2.Position;
			//bool edge2Clipped = ClipLine(clip5, clip6);


			//UINT16 insideCount = 0;
			//bool tv0inside = false;
			//bool tv1inside = false;
			//bool tv2inside = false;
			//UINT16 triangleCount = 0;
			//if (clip1.EqualsInTolerance(tv0.Position) && clip3.EqualsInTolerance(tv0.Position))
			//{
			//	tv0inside = true;
			//	insideCount++;
			//}
			//if (clip2.EqualsInTolerance(tv1.Position) && clip5.EqualsInTolerance(tv1.Position))
			//{
			//	tv1inside = true;
			//	insideCount++;
			//}
			//if (clip4.EqualsInTolerance(tv2.Position) && clip6.EqualsInTolerance(tv2.Position))
			//{
			//	tv2inside = true;
			//	insideCount++;
			//}

			//pMatrix = perspMat;
			//if (insideCount == 3) // �� �� ��� ���� ( �׸��� )
			//{
			//	triangleCount = 1;
			//	std::vector<Vector4> newVertices;
			//	newVertices.push_back(tv0.Position);
			//	newVertices.push_back(tv1.Position);
			//	newVertices.push_back(tv2.Position);
			//	DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());
			//}
			//else if (insideCount == 2) // �ﰢ�� �и�
			//{
			//	std::vector<Vector4> newVertices;
			//	if (tv0inside)
			//	{
			//		if (tv1inside)
			//		{
			//			newVertices.push_back(tv0.Position);
			//			newVertices.push_back(tv1.Position);
			//			newVertices.push_back(clip4);
			//			DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());

			//			newVertices.clear();
			//			newVertices.push_back(tv1.Position);
			//			newVertices.push_back(clip6);
			//			newVertices.push_back(clip4);
			//			DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());
			//		}
			//		else
			//		{
			//			newVertices.push_back(tv0.Position);
			//			newVertices.push_back(clip2);
			//			newVertices.push_back(clip5);
			//			DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());

			//			newVertices.clear();
			//			newVertices.push_back(tv0.Position);
			//			newVertices.push_back(clip5);
			//			newVertices.push_back(tv2.Position);
			//			DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());
			//		}
			//	}
			//	else
			//	{
			//		newVertices.push_back(tv2.Position);
			//		newVertices.push_back(clip3);
			//		newVertices.push_back(clip1);
			//		DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());

			//		newVertices.clear();
			//		newVertices.push_back(tv1.Position);
			//		newVertices.push_back(tv2.Position);
			//		newVertices.push_back(clip1);
			//		DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());
			//	}

			//	triangleCount = 2;
			//}
			//else if (insideCount == 1) // �ڸ� �ﰢ��
			//{
			//	std::vector<Vector4> newVertices;
			//	if (tv0inside)
			//	{
			//		newVertices.push_back(tv0.Position);
			//		newVertices.push_back(clip2);
			//		newVertices.push_back(clip4);
			//		DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());
			//	}
			//	else if (tv1inside)
			//	{
			//		newVertices.push_back(clip1);
			//		newVertices.push_back(tv1.Position);
			//		newVertices.push_back(clip6);
			//		DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());
			//	}
			//	else if (tv2inside)
			//	{
			//		newVertices.push_back(tv2.Position);
			//		newVertices.push_back(clip3);
			//		newVertices.push_back(clip5);
			//		DrawTriangle(tv0, tv1, tv2, newVertices, false, _GameEngine3.GetMainTexture());
			//	}
			//	triangleCount = 1;
			//}
			//else // �ǳʶٱ�
			//{
			//	triangleCount = 0;
			//}
		}

		renderedObjects++;
	}

	//_RSI->PushStatisticText("Total GameObjects : " + std::to_string(totalObjects));
	//_RSI->PushStatisticText("Culled GameObjects : " + std::to_string(culledObjects));
	//_RSI->PushStatisticText("Rendered GameObjects : " + std::to_string(renderedObjects));
}

void SoftRenderer::DrawTriangle(Vertex3D tv0, Vertex3D tv1, Vertex3D tv2, std::vector<Vector4>& vertices, bool DrawTexture, const Texture& InTexture)
{
	for (auto& v : vertices)
	{
		float invW = 1.f / v.W;
		v.X *= invW;
		v.Y *= invW;
		v.Z *= invW;

		// ȭ�� �������� Ȯ��
		v.X *= (_ScreenSize.X * 0.5f);
		v.Y *= (_ScreenSize.Y * 0.5f);
	}

	// �����̽� �ø� ( �޸��̸� �׸��� ���� )
	//Vector3 edge1 = (vertices[1] - vertices[0]).ToVector3();
	//Vector3 edge2 = (vertices[2] - vertices[0]).ToVector3();
	//if (edge1.Cross(edge2).Z > 0.f)
	//{
	//	return;
	//}

	// �ﰢ�� ĥ�ϱ�
	// �ﰢ���� ���� ����
	Vector2 minPos(Math::Min3(vertices[0].X, vertices[1].X, vertices[2].X), Math::Min3(vertices[0].Y, vertices[1].Y, vertices[2].Y));
	Vector2 maxPos(Math::Max3(vertices[0].X, vertices[1].X, vertices[2].X), Math::Max3(vertices[0].Y, vertices[1].Y, vertices[2].Y));

	// �����߽���ǥ�� ���� ���� ���ͷ� ��ȯ
	Vector2 u = vertices[1].ToVector2() - vertices[0].ToVector2();
	Vector2 v = vertices[2].ToVector2() - vertices[0].ToVector2();

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
			Vector2 w = pointToTest - vertices[0].ToVector2();
			float wdotu = w.Dot(u);
			float wdotv = w.Dot(v);

			float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
			float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
			float oneMinusST = 1.f - s - t;
			if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
			{
				_RSI->DrawPoint(fragment, LinearColor::DimGray);
				//// �� ������ ������ �� ������ z��
				//float invZ0 = 1.f / tv0.Position.W;
				//float invZ1 = 1.f / tv1.Position.W;
				//float invZ2 = 1.f / tv2.Position.W;

				//// ���� ���������� ����� ���� �и�
				//float z = invZ0 * oneMinusST + invZ1 * s + invZ2 * t;
				//float invZ = 1.f / z;

				//// ���� ��꿡 ����� �� ( ���������̹Ƿ� ��->���� ������ �迭�� ���� )
				//float k = pMatrix[2][2];
				//float l = pMatrix[3][2];
				//float newDepth = invZ * k + l;
				////float newDepth = Math::Clamp(-perspMat[2][2] + perspMat[3][2] * z, 0.f, 1.f);
				//float prevDepth = _RSI->GetDepthBufferValue(fragment);
				//if (newDepth < prevDepth)
				//{
				//	_RSI->SetDepthBufferValue(fragment, newDepth);
				//}
				//else
				//{
				//	// �̹� �տ� ���� �׷��������Ƿ� �ȼ��׸���� ����
				//	continue;
				//}

				//if (_ShowDepthBuffer)
				//{
				//	// �ð�ȭ�� ���� ����ȭ�� ��� ��
				//	// f = l / (k+1)
				//	// n = l / (k-1)
				//	float f = l / (k + 1.f);
				//	float n = l / (k - 1.f);
				//	float grayScale = (invZ - n) / (f - n);

				//	// ���� ���� �׸���
				//	_RSI->DrawPoint(fragment, LinearColor::White * grayScale);
				//}
				//else
				//{
				//	if (DrawTexture)
				//	{
				//		// ���������������� ������ �ش� �ȼ��� UV ��
				//		Vector2 targetUV = (tv0.UV * oneMinusST * invZ0 + tv1.UV * s * invZ1 + tv2.UV * t * invZ2) * invZ;

				//		// �ؽ��� ���� ����
				//		_RSI->DrawPoint(fragment, FragmentShader3D(InTexture.GetSample(targetUV)));
				//	}
				//	else
				//	{
				//		_RSI->DrawPoint(fragment, LinearColor::Blue);
				//	}
				//}
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
			return false;
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
		else if ((t < 0.f && clip1.Y < -clip1.W) || (t > 1.f && clip2.Y < -clip2.W)) // �� �� ��� �ٱ��� ����. Ŭ���� �ʿ� ����.
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
		else if ((t < 0.f && (clip1.X + clip1.W) < 0.f) || (t > 1.f && (clip2.X + clip2.W) < 0.f)) // �� �� ��� �ٱ��� ����. Ŭ���� �ʿ� ����.
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
		else if ((t < 0.f && clip1.Z < -clip1.W) || (t > 1.f && clip2.X < -clip2.W)) // �� �� ��� �ٱ��� ����. Ŭ���� �ʿ� ����.
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

