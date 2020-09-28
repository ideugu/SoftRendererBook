
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
	// 정점 변환 코드
	FORCEINLINE void VertexShader3D(std::vector<Vertex3D>& InVertices, Matrix4x4 InMatrix)
	{
		// 위치 값에 최종 행렬을 적용해 변환
		for (Vertex3D& v : InVertices)
		{
			v.Position = InMatrix * v.Position;
		}
	}

	LinearColor colorParam;

	// 픽셀 변환 코드
	FORCEINLINE LinearColor FragmentShader3D(LinearColor InColor)
	{
		return InColor;
	}
}


// 그리드 그리기
void SoftRenderer::DrawGizmo3D()
{
	// 뷰 기즈모 그리기
	std::vector<Vertex3D> viewGizmo = { 
		Vertex3D(Vector4(Vector3::Zero)),
		Vertex3D(Vector4(Vector3::UnitX * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitY * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitZ * _GizmoUnitLength)),
	};

	Matrix4x4 viewMatRotationOnly = _GameEngine3.GetMainCamera().GetViewMatrixRotationOnly();
	VertexShader3D(viewGizmo, viewMatRotationOnly);

	// 축 그리기
	Vector2 v0 = viewGizmo[0].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v1 = viewGizmo[1].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v2 = viewGizmo[2].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v3 = viewGizmo[3].Position.ToVector2() + _GizmoPositionOffset;
	_RSI->DrawLine(v0, v1, LinearColor::Red);
	_RSI->DrawLine(v0, v2, LinearColor::Green);
	_RSI->DrawLine(v0, v3, LinearColor::Blue);
}

// 게임 로직
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	static float moveSpeed = 500.f;
	static float fovSpeed = 100.f;

	InputManager input = _GameEngine3.GetInputManager();

	// 플레이어 게임 오브젝트 트랜스폼의 변경
	GameObject& player = _GameEngine3.FindGameObject(GameEngine::PlayerKey);
	if (!player.IsNotFound())
	{
		Transform& playerTransform = player.GetTransform();
		playerTransform.AddPosition(Vector3(input.GetXAxis(), input.GetWAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds);
		_GameEngine3.GetMainCamera().SetLookAtRotation(player.GetTransform().GetPosition());
	}

	// 카메라 FOV 조절
	Camera& camera = _GameEngine3.GetMainCamera();
	float newFOV = Math::Clamp(camera.GetFOV() + input.GetZAxis() * fovSpeed * InDeltaSeconds, 5.f, 179.f);
	camera.SetFOV(newFOV);

	// 버퍼 시각화 토글
	if (input.SpacePressed()) { _ShowDepthBuffer = !_ShowDepthBuffer; }
}

// 렌더링 로직
void SoftRenderer::Render3D()
{
	// 기즈모 그리기
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

	// 절두체 컬링을 위한 준비 작업. 행 벡터를 쉽게 구할 수 있게 전치시켜 둔다.
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

		// 동차좌표계를 사용해 절두체 컬링을 수행
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

		// 렌더러가 사용할 정점 버퍼와 인덱스 버퍼로 변환
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

		// 정점 변환 진행
		VertexShader3D(vertices, finalMat);

		// 삼각형 별로 그리기
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

			// 백페이스 컬링 ( 뒷면이면 그리기 생략 )
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

			if (nCountUnderW0 == 0) // 그대로 그리기
			{
				std::vector<Vertex3D> newVertices;
				newVertices.push_back(tv0);
				newVertices.push_back(tv1);
				newVertices.push_back(tv2);
				DrawTriangle(newVertices, mainTexture);
			}
			else if (nCountUnderW0 == 1) // 클리핑하기
			{
				Vertex3D underW0Pos;
				Vertex3D nonClippedPos1;
				Vertex3D nonClippedPos2;
				if (tv0UnderW0) // edge0와 edge1를 클리핑하기
				{
					underW0Pos = tv0;
					nonClippedPos1 = tv1;
					nonClippedPos2 = tv2;
				}
				else if (tv1UnderW0) // edge0와 edge2를 클리핑하기
				{
					underW0Pos = tv1;
					nonClippedPos1 = tv2;
					nonClippedPos2 = tv0;
				}
				else if (tv2UnderW0) // edge1와 edge2를 클리핑하기
				{
					underW0Pos = tv2;
					nonClippedPos1 = tv0;
					nonClippedPos2 = tv1;
				}

				// W=0까지 클리핑하기
				float t1 = underW0Pos.Position.W / (underW0Pos.Position.W - nonClippedPos1.Position.W);
				float t2 = underW0Pos.Position.W / (underW0Pos.Position.W - nonClippedPos2.Position.W);
				Vertex3D clippedPos1 = underW0Pos * (1.f - t1) + nonClippedPos1 * t1;
				Vertex3D clippedPos2 = underW0Pos * (1.f - t2) + nonClippedPos2 * t2;

				// 근평면까지 클리핑하기
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
			else if (nCountUnderW0 == 2) // 클리핑하기
			{
				Vertex3D underW0Pos1;
				Vertex3D underW0Pos2;
				Vertex3D nonClippedPos;
				if (!tv0UnderW0) // edge0와 edge1를 클리핑하기
				{
					nonClippedPos = tv0;
					underW0Pos1 = tv1;
					underW0Pos2 = tv2;
				}
				else if (!tv1UnderW0) // edge0와 edge2를 클리핑하기
				{
					nonClippedPos = tv1;
					underW0Pos1 = tv2;
					underW0Pos2 = tv0;
				}
				else if (!tv2UnderW0) // edge1과 edge2를 클리핑하기
				{
					nonClippedPos = tv2;
					underW0Pos1 = tv0;
					underW0Pos2 = tv1;
				}

				// W=0까지 클리핑하기
				float t1 = underW0Pos1.Position.W / (underW0Pos1.Position.W - nonClippedPos.Position.W);
				float t2 = underW0Pos2.Position.W / (underW0Pos2.Position.W - nonClippedPos.Position.W);
				Vertex3D clippedPos1 = underW0Pos1 * (1.f - t1) + nonClippedPos * t1;
				Vertex3D clippedPos2 = underW0Pos2 * (1.f - t2) + nonClippedPos * t2;

				// W=0에서 근평면까지 클리핑하기
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
		// 무한 원점인 경우, 약간 보정해준다.
		if (v.Position.W == 0.f) v.Position.W = SMALL_NUMBER;

		float invW = 1.f / v.Position.W;
		v.Position.X *= invW;
		v.Position.Y *= invW;
		v.Position.Z *= invW;

		// 화면 공간으로 확장
		v.Position.X *= (_ScreenSize.X * 0.5f);
		v.Position.Y *= (_ScreenSize.Y * 0.5f);
	}

	// 백페이스 컬링 ( 뒷면이면 그리기 생략 )
	Vector3 edge1 = (vertices[1].Position - vertices[0].Position).ToVector3();
	Vector3 edge2 = (vertices[2].Position - vertices[0].Position).ToVector3();
	float z = edge1.Cross(edge2).Z;
	if (z <= 0.f)
	{
		return;
	}

	// 삼각형 칠하기
	// 삼각형의 영역 설정
	Vector2 minPos(Math::Min3(vertices[0].Position.X, vertices[1].Position.X, vertices[2].Position.X), Math::Min3(vertices[0].Position.Y, vertices[1].Position.Y, vertices[2].Position.Y));
	Vector2 maxPos(Math::Max3(vertices[0].Position.X, vertices[1].Position.X, vertices[2].Position.X), Math::Max3(vertices[0].Position.Y, vertices[1].Position.Y, vertices[2].Position.Y));

	// 무게중심좌표를 위해 점을 벡터로 변환
	Vector2 u = vertices[1].Position.ToVector2() - vertices[0].Position.ToVector2();
	Vector2 v = vertices[2].Position.ToVector2() - vertices[0].Position.ToVector2();

	// 공통 분모 값 ( uu * vv - uv * uv )
	float udotv = u.Dot(v);
	float vdotv = v.Dot(v);
	float udotu = u.Dot(u);
	float denominator = udotv * udotv - vdotv * udotu;

	// 퇴화 삼각형의 판정
	if (Math::EqualsInTolerance(denominator, 0.0f))
	{
		// 퇴화 삼각형이면 건너뜀.
		return;
	}
	float invDenominator = 1.f / denominator;

	// 화면상의 점 구하기
	ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
	ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

	// 두 점이 화면 밖을 벗어나는 경우 클리핑 처리
	lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
	lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
	upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
	upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

	// 삼각형 영역 내 모든 점을 점검하고 색칠
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
				// 각 점마다 보존된 뷰 공간의 z값
				float invZ0 = 1.f / vertices[0].Position.W;
				float invZ1 = 1.f / vertices[1].Position.W;
				float invZ2 = 1.f / vertices[2].Position.W;

				// 투영 보정보간에 사용할 공통 분모
				float z = invZ0 * oneMinusST + invZ1 * s + invZ2 * t;
				float invZ = 1.f / z;

				// 뎁스 계산에 사용할 값 ( 열기반행렬이므로 열->행의 순으로 배열이 진행 )
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
					// 이미 앞에 무언가 그려져있으므로 픽셀그리기는 생략
					continue;
				}

				if (_ShowDepthBuffer)
				{
					// 시각화를 위해 선형화된 흑백 값
					// f = l / (k+1)
					// n = l / (k-1)
					float grayScale = (invZ - n) / (f - n);

					// 뎁스 버퍼 그리기
					_RSI->DrawPoint(fragment, LinearColor::White * grayScale);
				}
				else
				{
					// 투영보정보간으로 보간한 해당 픽셀의 UV 값
					Vector2 targetUV = (vertices[0].UV * oneMinusST * invZ0 + vertices[1].UV * s * invZ1 + vertices[2].UV * t * invZ2) * invZ;

					// 텍스쳐 매핑 진행
					_RSI->DrawPoint(fragment, FragmentShader3D(InTexture.GetSample(targetUV)));
				}
			}
		}
	}
}

