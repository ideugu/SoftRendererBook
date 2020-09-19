
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

	// ȸ�� �� �׸���
	static float axisLength = 100.f;
	Vector2 axisTo = (viewMatRotationOnly * _RotationAxis).ToVector2() * axisLength;
	Vector2 axisFrom = -axisTo;

	// ������ �� �׸���
	Vector2 orthoAxisTo = (viewMatRotationOnly * _RotationOrthoAxis).ToVector2() * axisLength * 0.5f;
	Vector2 orthoAxisFrom = -orthoAxisTo;

	_RSI->DrawLine(orthoAxisFrom, orthoAxisTo, LinearColor::Blue);
	_RSI->DrawLine(axisFrom, axisTo, LinearColor::Red);
}

// ���� ����
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	static float rotateSpeed = 180.f;

	InputManager input = _GameEngine3.GetInputManager();

	// ȸ������ �ݴ�������� ����
	_AxisRotator.Yaw += -input.GetWAxis() * rotateSpeed * InDeltaSeconds;
	_AxisRotator.Pitch += -input.GetYAxis() * rotateSpeed * InDeltaSeconds;
	_AxisRotator.Roll += input.GetZAxis() * rotateSpeed * InDeltaSeconds;
	_AxisRotator.Clamp();
	float cy, sy, cp, sp, cr, sr;
	Math::GetSinCos(sy, cy, _AxisRotator.Yaw);
	Math::GetSinCos(sp, cp, _AxisRotator.Pitch);
	Math::GetSinCos(sr, cr, _AxisRotator.Roll);
	_RotationAxis = Vector3(-cy * sr + sy * sp * cr, cp * cr, sy * sr + cy * sp * cr);
	_RotationOrthoAxis = Vector3(cy * cr + sy * sp * sr, cp * sr, -sy * cr + cy * sp * sr);

	// �÷��̾� ���� ������Ʈ Ʈ�������� ����
	GameObject& player = _GameEngine3.FindGameObject(GameEngine::PlayerKey);
	if (!player.IsNotFound())
	{
		_RotationDegree = Math::FMod(_RotationDegree + input.GetXAxis() * rotateSpeed * InDeltaSeconds, 360.f);
	}

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

	Matrix4x4 viewMat = _GameEngine3.GetMainCamera().GetViewMatrix();

	for (auto it = _GameEngine3.SceneBegin(); it != _GameEngine3.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *it;
		const Mesh& mesh = _GameEngine3.GetMesh(gameObject.GetMeshKey());
		const Transform& transform = gameObject.GetTransformConst();

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

		for (Vertex3D& v : vertices)
		{
			float sin, cos;
			Math::GetSinCos(sin, cos, _RotationDegree);
			Vector3 u = v.Position.ToVector3();
			float udotn = u.Dot(_RotationAxis);
			Vector3 ncrossu = _RotationAxis.Cross(u);
			Vector3 result = Vector3(u * cos + _RotationAxis * ((1.f - cos) * udotn) + ncrossu * sin) * transform.GetScale();
			v.Position = viewMat * Vector4(result);
		}

		// ��ȯ�� ������ �մ� �� �׸���
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
			Vertex3D& tv0 = vertices[indice[bi0]];
			Vertex3D& tv1 = vertices[indice[bi1]];
			Vertex3D& tv2 = vertices[indice[bi2]];

			// �� ���� ��� ī�޶� �ڿ� ������ �׸��� ����
			if (tv0.Position.Z < 0.f && tv1.Position.Z < 0.f && tv2.Position.Z < 0.f)
			{
				continue;
			}

			// �����̽� �ø� ( �޸��̸� �׸��� ���� )
			Vector3 edge1 = (tv1.Position - tv0.Position).ToVector3();
			Vector3 edge2 = (tv2.Position - tv0.Position).ToVector3();
			if (edge1.Cross(edge2).Z >= 0.f)
			{
				continue;
			}

			// ���� ������Ʈ�� ���� ����
			LinearColor objectColor = FragmentShader3D(gameObject.GetColor());

			// ���̾������� �׸���
			_RSI->DrawLine(tv0.Position, tv1.Position, objectColor);
			_RSI->DrawLine(tv0.Position, tv2.Position, objectColor);
			_RSI->DrawLine(tv1.Position, tv2.Position, objectColor);
		}

		_RSI->PushStatisticText("Rotation Axis : " + _RotationAxis.ToString());
		_RSI->PushStatisticText("Rotation : " + std::to_string(_RotationDegree) + "(deg)");
	}

}

