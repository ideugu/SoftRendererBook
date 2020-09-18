
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
	std::vector<Vertex3D> gizmosInSW = { 
		Vertex3D(Vector4(Vector3::Zero)),
		Vertex3D(Vector4(Vector3::UnitX * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitY * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitZ * _GizmoUnitLength)),
	};

	Matrix4x4 finalMat = _GameEngine3.GetMainCamera().GetViewMatrix();
	VertexShader3D(gizmosInSW, finalMat);

	static Vector4 gizmoPositionOffset(-320.f, -250.f, 0.f, 0.f);

	// X�� �׸���
	_RSI->DrawLine(gizmosInSW[0].Position + gizmoPositionOffset, gizmosInSW[1].Position + gizmoPositionOffset, LinearColor::Red);

	// Y�� �׸���
	_RSI->DrawLine(gizmosInSW[0].Position + gizmoPositionOffset, gizmosInSW[2].Position + gizmoPositionOffset, LinearColor::Green);

	// Z�� �׸���
	_RSI->DrawLine(gizmosInSW[0].Position + gizmoPositionOffset, gizmosInSW[3].Position + gizmoPositionOffset, LinearColor::Blue);
}


// ���� ����
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	static float moveSpeed = 100.f;
	static float rotateSpeed = 180.f;
	static float scaleSpeed = 100.f;

	InputManager input = _GameEngine3.GetInputManager();

	// �÷��̾� ���� ������Ʈ Ʈ�������� ����
	GameObject& player = _GameEngine3.FindGameObject(GameEngine::PlayerKey);
	if (!player.IsNotFound())
	{
		Transform& playerTransform = player.GetTransform();
		playerTransform.AddYawRotation(input.GetZAxis() * rotateSpeed * InDeltaSeconds);
		playerTransform.AddPitchRotation(-input.GetWAxis() * rotateSpeed * InDeltaSeconds);
		playerTransform.AddPosition(Vector3(0.f, 0.f, input.GetXAxis() * moveSpeed * InDeltaSeconds));
		float newScale = Math::Clamp(playerTransform.GetScale().X + scaleSpeed * input.GetYAxis() * InDeltaSeconds, 15.f, 500.f);
		playerTransform.SetScale(Vector3::One * newScale);
	}
}

// ������ ����
void SoftRenderer::Render3D()
{
	// ���� �׸���
	DrawGizmo3D();

	Matrix4x4 viewMat = _GameEngine3.GetMainCamera().GetViewMatrix();
	const Texture& steveTexture = _GameEngine3.GetMainTexture();

	for (auto it = _GameEngine3.SceneBegin(); it != _GameEngine3.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *it;
		const Mesh& mesh = _GameEngine3.GetMesh(gameObject.GetMeshKey());
		const Transform& transform = gameObject.GetTransformConst();
		Matrix4x4 finalMat = viewMat * transform.GetModelingMatrix();

		size_t vertexCount = mesh._Vertices.size();
		size_t indexCount = mesh._Indices.size();
		size_t triangleCount = indexCount / 3;

		// �������� ����� ���� ���ۿ� �ε��� ���۷� ��ȯ
		std::vector<Vertex3D> vertices(vertexCount);
		std::vector<int> indice(mesh._Indices);
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi].Position = Vector4(mesh._Vertices[vi]);
			vertices[vi].UV = mesh._UVs[vi];
		}

		// ���� ��ȯ ����
		VertexShader3D(vertices, finalMat);

		// ��ȯ�� ������ �մ� �� �׸���
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
			Vertex3D& tv0 = vertices[indice[bi0]];
			Vertex3D& tv1 = vertices[indice[bi1]];
			Vertex3D& tv2 = vertices[indice[bi2]];

			LinearColor lineColor = FragmentShader3D(gameObject.GetColor());

			_RSI->DrawLine(tv0.Position, tv1.Position, lineColor);
			_RSI->DrawLine(tv0.Position, tv2.Position, lineColor);
			_RSI->DrawLine(tv1.Position, tv2.Position, lineColor);
		}
	}
}

