
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
FORCEINLINE void VertexShader3D(std::vector<Vertex3D>& InVertices, const Matrix4x4& InMatrix)
{
	// ��ġ ���� ���� ����� ������ ��ȯ
	for (Vertex3D& v : InVertices)
	{
		v.Position = InMatrix * v.Position;
	}
}

// �ȼ� ��ȯ �ڵ�
FORCEINLINE LinearColor FragmentShader3D(LinearColor& InColor, const LinearColor& InColorParam)
{
	return InColor * InColorParam;
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
	// �⺻ ���۷���
	GameEngine& g = GetGameEngine();
	const InputManager& input = g.GetInputManager();
	Camera& camera = g.GetMainCamera();

	// �⺻ ���� ����
	static float fovSpeed = 100.f;
	static float rotateSpeed = 180.f;
	static float moveSpeed = 500.f;

	// ���� �������� ����� ���� ������Ʈ ���۷���
	GameObject& goPlayer = g.GetGameObject(GameEngine::PlayerGo);
	GameObject& goCameraRig = g.GetGameObject(GameEngine::CameraRigGo);

	goPlayer.GetTransform().AddLocalYawRotation(-input.GetAxis(InputAxis::XAxis) * rotateSpeed * InDeltaSeconds);
	goPlayer.GetTransform().AddLocalPosition(goPlayer.GetTransform().GetLocalZ() * input.GetAxis(InputAxis::YAxis) * moveSpeed * InDeltaSeconds);

	// ī�޶� ȭ�� ����
	float newFOV = Math::Clamp(camera.GetFOV() + input.GetAxis(InputAxis::ZAxis) * fovSpeed * InDeltaSeconds, 5.f, 179.f);
	camera.SetFOV(newFOV);
}

// ĳ���� �ִϸ��̼� ����
void SoftRenderer::LateUpdate3D(float InDeltaSeconds)
{
	// �⺻ ���۷���
	GameEngine& g = GetGameEngine();

	// �⺻ ���� ����
	static float elapsedTime = 0.f;
	static float neckLength = 5.f;
	static float armLegLength = 0.7f;
	static float neckDegree = 15.f;
	static float armLegDegree = 30.f;
	elapsedTime += InDeltaSeconds;

	// �ִϸ��̼��� ���� Ŀ�� ���� 
	float armLegCurrent = Math::FMod(elapsedTime, armLegLength) * Math::TwoPI / armLegLength;
	float neckCurrent = Math::FMod(elapsedTime, neckLength) * Math::TwoPI / neckLength;

	float armLegCurve = sinf(armLegCurrent) * armLegDegree;
	float neckCurve = sinf(neckCurrent) * neckDegree;

	// ĳ���� ���۷���
	GameObject& goPlayer = g.GetGameObject(GameEngine::PlayerGo);

	// ĳ���� �޽�
	Mesh& m = g.GetMesh(goPlayer.GetMeshKey());

	// ���� ȸ��
	Bone& neckBone = m.GetBone(GameEngine::NeckBone);
	neckBone.GetTransform().SetLocalRotation(Rotator(neckCurve, 0.f, 0.f));

	// ���� ȸ��
	Bone& leftArmBone = m.GetBone(GameEngine::LeftArmBone);
	leftArmBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, armLegCurve));

	Bone& rightArmBone = m.GetBone(GameEngine::RightArmBone);
	rightArmBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, -armLegCurve));

	// �ٸ��� ȸ��
	Bone& leftLegBone = m.GetBone(GameEngine::LeftLegBone);
	leftLegBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, armLegCurve));

	Bone& rightLegBone = m.GetBone(GameEngine::RightLegBone);
	rightLegBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, -armLegCurve));
}

// ������ ����
void SoftRenderer::Render3D()
{
	const GameEngine& g = GetGameEngineC();

	const Camera& mainCamera = g.GetMainCamera();
	const Matrix4x4 viewMat = mainCamera.GetViewMatrix();
	const Matrix4x4 perspMat = mainCamera.GetPerspectiveMatrix();
	const Matrix4x4 pvMat = perspMat * viewMat;
	const ScreenPoint viewportSize = mainCamera.GetViewportSize();
	float nearZ = mainCamera.GetNearZ();
	float farZ = mainCamera.GetFarZ();

	// ����� �׸���
	DrawGizmo3D();

	// �ٴ� �����
	ShowMode prevShowMode = _CurrentShowMode;
	_CurrentShowMode = ShowMode::Wireframe;
	static float planeScale = 100.f;
	const Mesh& planeMesh = g.GetMesh(GameEngine::PlaneMesh);
	TransformData pt(Vector3::Zero, Quaternion::Identity, Vector3::One * planeScale);
	DrawMesh(planeMesh, pvMat * pt.GetMatrix(), LinearColor::DimGray);
	_CurrentShowMode = prevShowMode;

	for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *(*it);
		const Transform& transform = gameObject.GetTransform();
		if (!gameObject.HasMesh() || !gameObject.IsVisible())
		{
			continue;
		}

		const Mesh& m = g.GetMesh(gameObject.GetMeshKey());
		// ��Ű���̰� WireFrame�� ��� ���� �׸���
		if (m.IsSkinnedMesh() && _CurrentShowMode == ShowMode::Wireframe)
		{
			ShowMode prevShowMode = _CurrentShowMode;
			_CurrentShowMode = ShowMode::Wireframe;

			const Mesh& boneMesh = g.GetMesh(GameEngine::ArrowMesh);
			for (const auto& b : m.GetBones())
			{
				if (!b.second.HasParent())
				{
					continue;
				}
				const Bone& bone = b.second;
				const Bone& parentBone = m.GetBone(bone.GetParentName());
				const TransformData& tGameObject = transform.GetWorldTransform();

				// �𵨸� ���������� ���� ��ġ
				const TransformData& t1 = parentBone.GetTransform().GetWorldTransform();
				const TransformData& t2 = bone.GetTransform().GetWorldTransform();

				// ���� ���� ���������� ���� ��ġ
				const TransformData& wt1 = t1.LocalToWorld(tGameObject);
				const TransformData& wt2 = t2.LocalToWorld(tGameObject);

				Vector3 boneVector = wt2.GetPosition() - wt1.GetPosition();
				TransformData tboneObject(wt1.GetPosition(), Quaternion(boneVector), Vector3(10.f, 10.f, boneVector.Size()));
				Matrix4x4 boneMatrix = pvMat * tboneObject.GetMatrix();
				DrawMesh(boneMesh, boneMatrix, LinearColor::Red);
			}

			_CurrentShowMode = prevShowMode;
		}

		// ���� ��ȯ ���
		Matrix4x4 finalMatrix = pvMat * transform.GetWorldMatrix();
		DrawMesh(m, finalMatrix, gameObject.GetColor());
	}
}

void SoftRenderer::DrawMesh(const Mesh& InMesh, const Matrix4x4& InMatrix, const LinearColor& InColor)
{
	size_t vertexCount = InMesh._Vertices.size();
	size_t indexCount = InMesh._Indices.size();
	size_t triangleCount = indexCount / 3;

	// �������� ����� ���� ���ۿ� �ε��� ���۷� ��ȯ
	std::vector<Vertex3D> vertices(vertexCount);
	std::vector<size_t> indice(InMesh._Indices);
	for (size_t vi = 0; vi < vertexCount; ++vi)
	{
		vertices[vi].Position = Vector4(InMesh._Vertices[vi]);

		//���̷�Ż �޽ø� ��Ű�� �۾� ����
		if (InMesh.IsSkinnedMesh())
		{
			Vector4 totalPosition = Vector4::Zero;
			Weight w = InMesh._Weights[vi];
			for (size_t wi = 0; wi < InMesh._ConnectedBones[vi]; ++wi)
			{
				std::string boneName = w.Bones[wi];
				if (InMesh.HasBone(boneName))
				{
					const Bone& b = InMesh.GetBone(boneName);
					const TransformData& t = b.GetTransform().GetWorldTransform();  // ���� ����
					const TransformData& bindPose = b.GetBindPose(); // ���� ����

					// BindPose ������ �߽����� Bone�� ���� ������ ���
					TransformData boneLocal = t.WorldToLocal(bindPose);

					// BindPose �������� ���� ��ȭ
					Vector3 localPosition = bindPose.Inverse().GetPosition() + vertices[vi].Position.ToVector3();

					// BindPose ���������� ���� ���� ��ġ
					Vector3 skinnedLocalPosition = boneLocal.GetMatrix() * localPosition;

					// ���� �������� �ٽ� ����
					Vector3 skinnedWorldPosition = bindPose.GetMatrix() * skinnedLocalPosition;

					// ����ġ�� ���ؼ� ������
					totalPosition += Vector4(skinnedWorldPosition * w.Values[wi], true);
				}
			}

			vertices[vi].Position = totalPosition;
		}

		if (InMesh.HasUV())
		{
			vertices[vi].UV = InMesh._UVs[vi];
		}

		if (InMesh.HasColor())
		{
			vertices[vi].Color = InMesh._Colors[vi];
		}
	}

	// ���� ��ȯ ����
	VertexShader3D(vertices, InMatrix);

	// �׸����� ����
	DrawMode dm = DrawMode::ColorOnly;
	if (InMesh.HasUV())
	{
		if (InMesh.HasColor())
		{
			dm = DrawMode::ColorAndTexture;
		}
		else
		{
			dm = DrawMode::TextureOnly;
		}
	}

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
			DrawTriangle(sub, InColor, dm);
		}
	}
}

void SoftRenderer::DrawTriangle(std::vector<Vertex3D>& InVertices, const LinearColor& InColor, DrawMode InDrawMode)
{
	const GameEngine& g = GetGameEngineC();

	for (auto& v : InVertices)
	{
		// ���� ������ ���, �ణ �������ش�.
		if (v.Position.W == 0.f) v.Position.W = SMALL_NUMBER;

		float invW = 1.f / v.Position.W;
		v.Position.X *= invW;
		v.Position.Y *= invW;
		v.Position.Z *= invW;
	}

	// �����̽� �ø� ( �޸��̸� �׸��� ���� )
	Vector3 edge1 = (InVertices[1].Position - InVertices[0].Position).ToVector3();
	Vector3 edge2 = (InVertices[2].Position - InVertices[0].Position).ToVector3();
	float z = edge1.Cross(edge2).Z;
	if (z <= 0.f)
	{
		return;
	}

	if (_CurrentShowMode == ShowMode::Wireframe)
	{
		for (auto& v : InVertices)
		{
			v.Position.X *= _ScreenSize.X * 0.5f;
			v.Position.Y *= _ScreenSize.Y * 0.5f;
		}

		LinearColor finalColor = LinearColor::DimGray;
		if (InColor != LinearColor::White)
		{
			finalColor = InColor;
		}

		_RSI->DrawLine(InVertices[0].Position, InVertices[1].Position, finalColor);
		_RSI->DrawLine(InVertices[0].Position, InVertices[2].Position, finalColor);
		_RSI->DrawLine(InVertices[1].Position, InVertices[2].Position, finalColor);
		return;
	}

	// �ﰢ�� ĥ�ϱ�
	// �ﰢ���� ���� ����
	Vector2 minPos(Math::Min3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Min3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));
	Vector2 maxPos(Math::Max3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Max3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));

	// �����߽���ǥ�� ���� ���� ���ͷ� ��ȯ
	Vector2 u = InVertices[1].Position.ToVector2() - InVertices[0].Position.ToVector2();
	Vector2 v = InVertices[2].Position.ToVector2() - InVertices[0].Position.ToVector2();

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
			Vector2 w = pointToTest - InVertices[0].Position.ToVector2();
			float wdotu = w.Dot(u);
			float wdotv = w.Dot(v);

			float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
			float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
			float oneMinusST = 1.f - s - t;
			if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
			{
				// �� ������ ������ �� ������ z��
				float invZ0 = 1.f / InVertices[0].Position.W;
				float invZ1 = 1.f / InVertices[1].Position.W;
				float invZ2 = 1.f / InVertices[2].Position.W;

				// ���� ���������� ����� ���� �и�
				float z = invZ0 * oneMinusST + invZ1 * s + invZ2 * t;
				float invZ = 1.f / z;

				float n = g.GetMainCamera().GetNearZ();
				float f = g.GetMainCamera().GetFarZ();
				//float newDepth = (invZ - n) / (f - n);
				float newDepth = (InVertices[0].Position.Z * oneMinusST * invZ0 + InVertices[1].Position.Z * s * invZ1 + InVertices[2].Position.Z * t * invZ2) * invZ;
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
					LinearColor finalColor = LinearColor::White;
					if (InDrawMode == DrawMode::ColorOnly || InDrawMode == DrawMode::ColorAndTexture)
					{
						finalColor = (InVertices[0].Color * oneMinusST * invZ0 + InVertices[1].Color * s * invZ1 + InVertices[2].Color * t * invZ2) * invZ;
					}

					if (InDrawMode == DrawMode::TextureOnly || InDrawMode == DrawMode::ColorAndTexture)
					{
						// ���������������� ������ �ش� �ȼ��� UV ��
						Vector2 targetUV = (InVertices[0].UV * oneMinusST * invZ0 + InVertices[1].UV * s * invZ1 + InVertices[2].UV * t * invZ2) * invZ;

						// �ؽ��� ���� ����
						LinearColor textureColor = g.GetTexture(GameEngine::DiffuseTexture).GetSample(targetUV);
						finalColor = finalColor * textureColor;
					}

					_RSI->DrawPoint(fragment, FragmentShader3D(finalColor, InColor));
				}
			}
		}
	}
}

