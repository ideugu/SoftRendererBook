
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
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
	struct PerspectiveTest
	{
		std::function<bool(const Vertex3D& v)> testFn1;
		std::function<Vertex3D(const Vertex3D& vStart, const Vertex3D& vEnd)> testFn2;
		std::array<bool, 3> test1Result;

		void clipTriangles(std::vector<Vertex3D>& tvs)
		{
			size_t nTriangles = tvs.size() / 3;
			for (size_t ti = 0; ti < nTriangles; ++ti)
			{
				size_t si = ti * 3;
				size_t testNotPassedCount = 0;

				std::vector<Vertex3D> sub(tvs.begin() + si, tvs.begin() + si + 3);
				// 테스트에 실패한 점 정보 얻기
				for (size_t ix = 0; ix < 3; ++ix)
				{
					bool testResult = testFn1(sub[ix]);
					test1Result[ix] = testResult;
					if (testResult) testNotPassedCount++;
				}

				getNewVertices(sub, testNotPassedCount);

				if (testNotPassedCount == 0)
				{
					continue;
				}
				else if (testNotPassedCount == 1)  // 삼각형 추가
				{
					tvs[si] = sub[0];
					tvs[si + 1] = sub[1];
					tvs[si + 2] = sub[2];
					tvs.push_back(sub[3]);
					tvs.push_back(sub[4]);
					tvs.push_back(sub[5]);
				}
				else if (testNotPassedCount == 2) // 삼각형 정보 변경
				{
					tvs[si] = sub[0];
					tvs[si + 1] = sub[1];
					tvs[si + 2] = sub[2];
				}
				else // 삼각형을 목록에서 제거
				{
					tvs.erase(tvs.begin() + si, tvs.begin() + si + 3);
					nTriangles--;
					ti--;
				}
			}
		}

		// 패스하면 false, 사용할거면 true
		bool getNewVertices(std::vector<Vertex3D>& vertices, size_t testNotPassedCount)
		{
			if (testNotPassedCount == 0) // 그대로 통과
			{
				return true;
			}
			else if (testNotPassedCount == 1)
			{
				// Edge를 만든 후 클리핑 진행. 점이 두 개가 추가되고 삼각형이 2개로 쪼개짐
				UINT16 index = 0; // 테스트에 걸린 점의 인덱스
				if (!test1Result[0])
				{
					index = test1Result[1] ? 1 : 2;
				}
				Vertex3D v1 = vertices[(index + 1) % 3];
				Vertex3D v2 = vertices[(index + 2) % 3];
				Vertex3D clipped1 = testFn2(vertices[index], v1);
				Vertex3D clipped2 = testFn2(vertices[index], v2);
				vertices[0] = clipped1;
				vertices[1] = v1;
				vertices[2] = v2;
				vertices.push_back(clipped1);
				vertices.push_back(v2);
				vertices.push_back(clipped2);
				return true;
			}
			else if (testNotPassedCount == 2)
			{
				// Edge를 만든 후 클리핑 진행. 점이 두 개가 변경되고 삼각형은 그대로.
				UINT16 index = 0;  // 테스트에 걸리지 않은 점의 인덱스
				if (test1Result[0])
				{
					index = !test1Result[1] ? 1 : 2;
				}

				Vertex3D v1 = vertices[(index + 1) % 3];
				Vertex3D v2 = vertices[(index + 2) % 3];
				Vertex3D clipped1 = testFn2(vertices[index], v1);
				Vertex3D clipped2 = testFn2(vertices[index], v2);
				vertices[0] = vertices[index];
				vertices[1] = clipped1;
				vertices[2] = clipped2;
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	auto testW0 = [](const Vertex3D& v) {
		return v.Position.W < 0.f;
	};

	auto edgeW0 = [](const Vertex3D& vStart, const Vertex3D& vEnd) {
		float p1 = vStart.Position.W;
		float p2 = vEnd.Position.W;
		float t = p1 / (p1 - p2);
		return vStart * (1.f - t) + vEnd * t;
	};

	auto testNY = [](const Vertex3D& v) {
		return v.Position.Y < -v.Position.W;
	};

	auto edgeNY = [](const Vertex3D& vStart, const Vertex3D& vEnd) {
		float p1 = vStart.Position.W + vStart.Position.Y;
		float p2 = vEnd.Position.W + vEnd.Position.Y;
		float t = p1 / (p1 - p2);
		return vStart * (1.f - t) + vEnd * t;
	};

	auto testPY = [](const Vertex3D& v) {
		return v.Position.Y > v.Position.W;
	};

	auto edgePY = [](const Vertex3D& vStart, const Vertex3D& vEnd) {
		float p1 = vStart.Position.W - vStart.Position.Y;
		float p2 = vEnd.Position.W - vEnd.Position.Y;
		float t = p1 / (p1 - p2);
		return vStart * (1.f - t) + vEnd * t;
	};

	auto testNX = [](const Vertex3D& v) {
		return v.Position.X < -v.Position.W;
	};

	auto edgeNX = [](const Vertex3D& vStart, const Vertex3D& vEnd) {
		float p1 = vStart.Position.W + vStart.Position.X;
		float p2 = vEnd.Position.W + vEnd.Position.X;
		float t = p1 / (p1 - p2);
		return vStart * (1.f - t) + vEnd * t;
	};

	auto testPX = [](const Vertex3D& v) {
		return v.Position.X > v.Position.W;
	};

	auto edgePX = [](const Vertex3D& vStart, const Vertex3D& vEnd) {
		float p1 = vStart.Position.W - vStart.Position.X;
		float p2 = vEnd.Position.W - vEnd.Position.X;
		float t = p1 / (p1 - p2);
		return vStart * (1.f - t) + vEnd * t;
	};

	auto testF = [](const Vertex3D& v) {
		return v.Position.Z > v.Position.W;
	};

	auto edgeF = [](const Vertex3D& vStart, const Vertex3D& vEnd) {
		float p1 = vStart.Position.W - vStart.Position.Z;
		float p2 = vEnd.Position.W - vEnd.Position.Z;
		float t = p1 / (p1 - p2);
		return vStart * (1.f - t) + vEnd * t;
	};

	auto testN = [](const Vertex3D& v) {
		return v.Position.Z < -v.Position.W;
	};

	auto edgeN = [](const Vertex3D& vStart, const Vertex3D& vEnd) {
		float p1 = vStart.Position.W + vStart.Position.Z;
		float p2 = vEnd.Position.W + vEnd.Position.Z;
		float t = p1 / (p1 - p2);
		return vStart * (1.f - t) + vEnd * t;
	};


	// 정점 변환 코드
	FORCEINLINE void VertexShader3D(std::vector<Vertex3D>& InVertices, const Matrix4x4& InMatrix)
	{
		// 위치 값에 최종 행렬을 적용해 변환
		for (Vertex3D& v : InVertices)
		{
			v.Position = InMatrix * v.Position;
		}
	}

	// 픽셀 변환 코드
	FORCEINLINE LinearColor FragmentShader3D(LinearColor& InColor, const LinearColor& InColorParam)
	{
		return InColor * InColorParam;
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
	// 기본 레퍼런스
	GameEngine& g = GetGameEngine();
	const InputManager& input = g.GetInputManager();
	Camera& camera = g.GetMainCamera();

	// 기본 설정 변수
	static float elapsedTime = 0.f;
	static float duration = 5.f;
	static float fovSpeed = 100.f;

	// 카메라 화각 설정
	float newFOV = Math::Clamp(camera.GetFOV() + input.GetAxis(InputAxis::ZAxis) * fovSpeed * InDeltaSeconds, 5.f, 179.f);
	camera.SetFOV(newFOV);
}

// 렌더링 로직
void SoftRenderer::Render3D()
{
	const GameEngine& g = GetGameEngineC();

	// 기즈모 그리기
	DrawGizmo3D();

	const Camera& mainCamera = g.GetMainCamera();
	Matrix4x4 viewMat = mainCamera.GetViewMatrix();
	Matrix4x4 perspMat = mainCamera.GetPerspectiveMatrix();
	Matrix4x4 pvMat = perspMat * viewMat;
	ScreenPoint viewportSize = mainCamera.GetViewportSize();
	float nearZ = mainCamera.GetNearZ();
	float farZ = mainCamera.GetFarZ();

	for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *(*it);
		const TransformNode& transformNode = gameObject.GetTransformNode();

		if (!gameObject.HasMesh())
		{
			continue;
		}

		const Mesh& m = g.GetMesh(gameObject.GetMeshKey());

		size_t vertexCount = m._Vertices.size();
		size_t indexCount = m._Indices.size();
		size_t triangleCount = indexCount / 3;
		bool hasUV = m._UVs.size() > 0;

		// 렌더러가 사용할 정점 버퍼와 인덱스 버퍼로 변환
		std::vector<Vertex3D> vertices(vertexCount);
		std::vector<int> indice(m._Indices);
		for (size_t vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi].Position = Vector4(m._Vertices[vi]);
			
			// 위치에 대해 스키닝 연산 수행
			//if (m.IsSkinnedMesh())
			//{
			//	Vector3 deltaPosition;
			//	Weight w = m._Weights[vi];
			//	for (size_t wi = 0; wi < m._ConnectedBones[vi]; ++wi)
			//	{
			//		std::string boneName = w.Bones[wi];
			//		if (m.HasBone(boneName))
			//		{
			//			const Transform& boneTransform = m.GetBoneTransform(boneName);
			//			deltaPosition += boneTransform.GetLocalPosition() * w.Values[wi];
			//		}
			//	}

			//	vertices[vi].Position += Vector4(deltaPosition, false);
			//}

			if(hasUV)
			{
				vertices[vi].UV = m._UVs[vi];
			}
		}

		// 최종 변환 행렬
		Matrix4x4 finalMatrix = pvMat * transformNode.GetWorldMatrix();

		// 최종 색상
		LinearColor finalColor = LinearColor::White;

		// 정점 변환 진행
		VertexShader3D(vertices, finalMatrix);

		// 삼각형 별로 그리기
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
			std::vector<Vertex3D> tvs = { vertices[indice[bi0]] , vertices[indice[bi1]] , vertices[indice[bi2]] };

			// 동차좌표계에서 클리핑을 위한 설정
			std::vector<PerspectiveTest> testVectors = {
				{ testW0, edgeW0 },
				{ testNY, edgeNY },
				{ testPY, edgePY },
				{ testNX, edgeNX },
				{ testPX, edgePX },
				{ testF, edgeF },
				{ testN, edgeN }
			};

			// 동차좌표계에서 클리핑 진행
			for (auto& p : testVectors)
			{
				p.clipTriangles(tvs);
			}

			size_t nTriangles = tvs.size() / 3;
			for (size_t ti = 0; ti < nTriangles; ++ti)
			{
				size_t si = ti * 3;
				std::vector<Vertex3D> sub(tvs.begin() + si, tvs.begin() + si + 3);
				DrawTriangle(sub, finalColor);
			}
		}
	}
}

void SoftRenderer::DrawTriangle(std::vector<Vertex3D>& vertices, const LinearColor& InColorParam)
{
	const GameEngine& g = GetGameEngineC();

	for (auto& v : vertices)
	{
		// 무한 원점인 경우, 약간 보정해준다.
		if (v.Position.W == 0.f) v.Position.W = SMALL_NUMBER;

		float invW = 1.f / v.Position.W;
		v.Position.X *= invW;
		v.Position.Y *= invW;
		v.Position.Z *= invW;
	}

	// 백페이스 컬링 ( 뒷면이면 그리기 생략 )
	Vector3 edge1 = (vertices[1].Position - vertices[0].Position).ToVector3();
	Vector3 edge2 = (vertices[2].Position - vertices[0].Position).ToVector3();
	float z = edge1.Cross(edge2).Z;
	if (z <= 0.f)
	{
		return;
	}

	if (_CurrentShowMode == ShowMode::Wireframe)
	{
		for (auto& v : vertices)
		{
			v.Position.X *= _ScreenSize.X * 0.5f;
			v.Position.Y *= _ScreenSize.Y * 0.5f;
		}

		_RSI->DrawLine(vertices[0].Position, vertices[1].Position, LinearColor::DimGray);
		_RSI->DrawLine(vertices[0].Position, vertices[2].Position, LinearColor::DimGray);
		_RSI->DrawLine(vertices[1].Position, vertices[2].Position, LinearColor::DimGray);
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

	if (denominator == 0.f)
	{
		// 퇴화 삼각형이면 건너뜀.
		return;
	}

	float invDenominator = 1.f / denominator;

	// 화면상의 점 구하기
	minPos.X *= _ScreenSize.X * 0.5f;
	minPos.Y *= _ScreenSize.Y * 0.5f;
	maxPos.X *= _ScreenSize.X * 0.5f;
	maxPos.Y *= _ScreenSize.Y * 0.5f;

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
			pointToTest.X *= (2.f / _ScreenSize.X);
			pointToTest.Y *= (2.f / _ScreenSize.Y);
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

				float n = g.GetMainCamera().GetNearZ();
				float f = g.GetMainCamera().GetFarZ();
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

				if (_CurrentShowMode == ShowMode::DepthBuffer)
				{
					// 시각화를 위해 선형화된 흑백 값
					float grayScale = (invZ - n) / (f - n);

					// 뎁스 버퍼 그리기
					_RSI->DrawPoint(fragment, LinearColor::White * grayScale);
				}
				else
				{
					// 투영보정보간으로 보간한 해당 픽셀의 UV 값
					Vector2 targetUV = (vertices[0].UV * oneMinusST * invZ0 + vertices[1].UV * s * invZ1 + vertices[2].UV * t * invZ2) * invZ;

					// 텍스쳐 매핑 진행
					_RSI->DrawPoint(fragment, FragmentShader3D(g.GetTexture(GameEngine::DiffuseTexture).GetSample(targetUV), InColorParam));
				}
			}
		}
	}
}

