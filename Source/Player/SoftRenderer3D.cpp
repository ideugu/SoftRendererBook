
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
				// �׽�Ʈ�� ������ �� ���� ���
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
				else if (testNotPassedCount == 1)  // �ﰢ�� �߰�
				{
					tvs[si] = sub[0];
					tvs[si + 1] = sub[1];
					tvs[si + 2] = sub[2];
					tvs.push_back(sub[3]);
					tvs.push_back(sub[4]);
					tvs.push_back(sub[5]);
				}
				else if (testNotPassedCount == 2) // �ﰢ�� ���� ����
				{
					tvs[si] = sub[0];
					tvs[si + 1] = sub[1];
					tvs[si + 2] = sub[2];
				}
				else // �ﰢ���� ��Ͽ��� ����
				{
					tvs.erase(tvs.begin() + si, tvs.begin() + si + 3);
					nTriangles--;
					ti--;
				}
			}
		}

		// �н��ϸ� false, ����ҰŸ� true
		bool getNewVertices(std::vector<Vertex3D>& vertices, size_t testNotPassedCount)
		{
			if (testNotPassedCount == 0) // �״�� ���
			{
				return true;
			}
			else if (testNotPassedCount == 1)
			{
				// Edge�� ���� �� Ŭ���� ����. ���� �� ���� �߰��ǰ� �ﰢ���� 2���� �ɰ���
				UINT16 index = 0; // �׽�Ʈ�� �ɸ� ���� �ε���
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
				// Edge�� ���� �� Ŭ���� ����. ���� �� ���� ����ǰ� �ﰢ���� �״��.
				UINT16 index = 0;  // �׽�Ʈ�� �ɸ��� ���� ���� �ε���
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


	// ���� ��ȯ �ڵ�
	FORCEINLINE void VertexShader3D(std::vector<Vertex3D>& InVertices, Matrix4x4 InMatrix)
	{
		// ��ġ ���� ���� ����� ������ ��ȯ
		for (Vertex3D& v : InVertices)
		{
			v.Position = InMatrix * v.Position;
		}
	}

	LinearColor colorParam = LinearColor::White;

	// �ȼ� ��ȯ �ڵ�
	FORCEINLINE LinearColor FragmentShader3D(LinearColor InColor)
	{
		return InColor * colorParam;
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
	static float duration = 5.f;
	static float fovSpeed = 100.f;
	static float elapsedTime = 0.f;

	// ���� �߻���
	static std::mt19937 generator(0);
	static std::uniform_real_distribution<float> dir(-1.f, 1.f);
	static std::uniform_real_distribution<float> angle(0.f, 180.f);

	const InputManager& input = _GameEngine3.GetInputManager();
	Camera& camera = _GameEngine3.GetMainCamera();

	// ���� ������ ���� ����� ����
	static Quaternion startRotation(camera.GetTransform().GetRotation());
	static Quaternion targetRotation(Rotator(90.f, 0.f, 0.f));

	elapsedTime = Math::Clamp(elapsedTime + InDeltaSeconds, 0.f, duration);
	if (elapsedTime == duration)
	{
		elapsedTime = 0.f;
		startRotation = targetRotation;

		Vector3 randomAxis(dir(generator), dir(generator), dir(generator));
		randomAxis.Normalize();
		targetRotation = Quaternion(randomAxis, angle(generator));
		camera.GetTransform().SetRotation(startRotation);
	}
	else
	{
		float t = elapsedTime / duration;
		Quaternion current = Quaternion::Slerp(startRotation, targetRotation, t);
		camera.GetTransform().SetRotation(current);
	}

	// ī�޶� ȭ�� ����
	float newFOV = Math::Clamp(camera.GetFOV() + input.GetAxis(InputAxis::ZAxis) * fovSpeed * InDeltaSeconds, 5.f, 179.f);
	camera.SetFOV(newFOV);
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
	size_t intersectedObjects = 0;
	size_t renderedObjects = 0;

	// ����ü �ø��� �����ϱ� ���� �⺻ ���� ��
	float halfFOV = mainCamera.GetFOV() * 0.5f;
	float pSin, pCos;
	Math::GetSinCos(pSin, pCos, halfFOV);

	// ����ü ����� ������
	static std::array<Plane, 6> frustumPlanesManual = {
		Plane(Vector3(pCos, 0.f, pSin), 0.f), // +Y
		Plane(Vector3(-pCos, 0.f, pSin), 0.f), // -Y
		Plane(Vector3(0.f, pCos, pSin), 0.f), // +X
		Plane(Vector3(0.f, -pCos, pSin), 0.f), // -X
		Plane(Vector3::UnitZ, nearZ), // +Z
		Plane(-Vector3::UnitZ, -farZ) // -Z
	};

	Frustum f1(frustumPlanesManual);

	// ����ü �ø��� ���� �غ� �۾�. �� ���͸� ���� ���� �� �ְ� ��ġ���� �д�.
	Matrix4x4 pvMatT = pvMat.Tranpose();
	std::array<Plane, 6> frustumPlanesFromMatrix = {
		-(pvMatT[3] - pvMatT[1]), // +Y
		-(pvMatT[3] + pvMatT[1]), // -Y
		-(pvMatT[3] - pvMatT[0]), // +X
		-(pvMatT[3] + pvMatT[0]), // -X 
		-(pvMatT[3] - pvMatT[2]),  // +Z
		-(pvMatT[3] + pvMatT[2]), // -Z
	};

	Frustum f2(frustumPlanesFromMatrix);

	const Texture& mainTexture = _GameEngine3.GetMainTexture();

	for (auto it = _GameEngine3.SceneBegin(); it != _GameEngine3.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *it;
		const Transform& transform = gameObject.GetReadOnlyTransform();

		// ������ǥ�踦 ����� ����ü �ø��� ����
		const Mesh& mesh = _GameEngine3.GetMesh(gameObject.GetMeshKey());

		Matrix4x4 finalMat = pvMat * transform.GetModelingMatrix();
		Matrix4x4 finalMatT = finalMat.Tranpose();
		std::array<Plane, 6> frustumPlanesFromMatrix2 = {
			-(finalMatT[3] - finalMatT[1]), // up
			-(finalMatT[3] + finalMatT[1]), // bottom
			-(finalMatT[3] - finalMatT[0]), // right
			-(finalMatT[3] + finalMatT[0]), // left 
			-(finalMatT[3] - finalMatT[2]),  // far
			-(finalMatT[3] + finalMatT[2]), // near
		};

		Frustum f3(frustumPlanesFromMatrix2);

		// �ٿ�� ������ ũ�⵵ Ʈ�������� �°� ����
		const Sphere& sphereBound = mesh.GetSphereBound();
		const Box& boxBound = mesh.GetBoxBound();

		auto checkResult = f3.CheckBound(boxBound);
		if (checkResult == BoundCheckResult::Outside)
		{
			culledObjects++;
			continue;
		}
		else if(checkResult == BoundCheckResult::Intersect)
		{
			intersectedObjects++;
			colorParam = LinearColor::Red;
		}
		else
		{
			colorParam = LinearColor::White;
		}

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
			std::vector<Vertex3D> tvs = { vertices[indice[bi0]] , vertices[indice[bi1]] , vertices[indice[bi2]] };

			// ������ǥ�迡�� Ŭ������ ���� ����
			std::vector<PerspectiveTest> testVectors = {
				{ testW0, edgeW0 },
				{ testNY, edgeNY },
				{ testPY, edgePY },
				{ testNX, edgeNX },
				{ testPX, edgePX },
				{ testF, edgeF },
				{ testN, edgeN }
			};

			// ������ǥ�迡�� Ŭ���� ����
			for (auto& p : testVectors)
			{
				p.clipTriangles(tvs);
			}

			size_t nTriangles = tvs.size() / 3;
			for (size_t ti = 0; ti < nTriangles; ++ti)
			{
				size_t si = ti * 3;
				std::vector<Vertex3D> sub(tvs.begin() + si, tvs.begin() + si + 3);
				DrawTriangle(sub);
			}
		}

		renderedObjects++;
	}

	_RSI->PushStatisticText("Total GameObjects : " + std::to_string(totalObjects));
	_RSI->PushStatisticText("Culled GameObjects : " + std::to_string(culledObjects));
	_RSI->PushStatisticText("Intersected GameObjects : " + std::to_string(intersectedObjects));
	_RSI->PushStatisticText("Rendered GameObjects : " + std::to_string(renderedObjects));
}

void SoftRenderer::DrawTriangle(std::vector<Vertex3D>& vertices)
{
	for (auto& v : vertices)
	{
		// ���� ������ ���, �ణ �������ش�.
		if (v.Position.W == 0.f) v.Position.W = SMALL_NUMBER;

		float invW = 1.f / v.Position.W;
		v.Position.X *= invW;
		v.Position.Y *= invW;
		v.Position.Z *= invW;
	}

	// �����̽� �ø� ( �޸��̸� �׸��� ���� )
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

	if (denominator == 0.f)
	{
		// ��ȭ �ﰢ���̸� �ǳʶ�.
		return;
	}

	float invDenominator = 1.f / denominator;

	// ȭ����� �� ���ϱ�
	minPos.X *= _ScreenSize.X * 0.5f;
	minPos.Y *= _ScreenSize.Y * 0.5f;
	maxPos.X *= _ScreenSize.X * 0.5f;
	maxPos.Y *= _ScreenSize.Y * 0.5f;

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
				// �� ������ ������ �� ������ z��
				float invZ0 = 1.f / vertices[0].Position.W;
				float invZ1 = 1.f / vertices[1].Position.W;
				float invZ2 = 1.f / vertices[2].Position.W;

				// ���� ���������� ����� ���� �и�
				float z = invZ0 * oneMinusST + invZ1 * s + invZ2 * t;
				float invZ = 1.f / z;

				float n = _GameEngine3.GetMainCamera().GetNearZ();
				float f = _GameEngine3.GetMainCamera().GetFarZ();
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

				if (_CurrentShowMode == ShowMode::DepthBuffer)
				{
					// �ð�ȭ�� ���� ����ȭ�� ��� ��
					float grayScale = (invZ - n) / (f - n);

					// ���� ���� �׸���
					_RSI->DrawPoint(fragment, LinearColor::White * grayScale);
				}
				else
				{
					// ���������������� ������ �ش� �ȼ��� UV ��
					Vector2 targetUV = (vertices[0].UV * oneMinusST * invZ0 + vertices[1].UV * s * invZ1 + vertices[2].UV * t * invZ2) * invZ;

					// �ؽ��� ���� ����
					_RSI->DrawPoint(fragment, FragmentShader3D(_GameEngine3.GetMainTexture().GetSample(targetUV)));
				}
			}
		}
	}
}

