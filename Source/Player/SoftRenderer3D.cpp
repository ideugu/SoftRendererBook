
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

FORCEINLINE void Apply3DTransform(std::vector<Vertex3D>& InVertices, Matrix4x4 InMatrix, ScreenPoint InScreenSize)
{
	for (Vertex3D& v : InVertices)
	{
		v.Position = InMatrix * v.Position;
		float invW = 1.f / v.Position.W;
		v.Position.X *= invW;
		v.Position.Y *= invW;
		v.Position.Z *= invW;
		v.Position.W = invW;

		v.Position.X *= (InScreenSize.X * 0.5f);
		v.Position.Y *= (InScreenSize.Y * 0.5f);
	}
}

// 그리드 그리기
void SoftRenderer::DrawGizmo3D()
{
	std::vector<Vertex3D> gizmos = { 
		Vertex3D(Vector4(Vector3::Zero)),
		Vertex3D(Vector4(Vector3::UnitX * _GizmoUnitLength)),
		Vertex3D(Vector4(-Vector3::UnitX * _GizmoWorldAxesLength)),
		Vertex3D(Vector4(Vector3::UnitX * _GizmoWorldAxesLength)),
		Vertex3D(Vector4(Vector3::UnitY * _GizmoUnitLength)),
		Vertex3D(Vector4(-Vector3::UnitY * _GizmoWorldAxesLength)),
		Vertex3D(Vector4(Vector3::UnitY * _GizmoWorldAxesLength)),
		Vertex3D(Vector4(Vector3::UnitZ * _GizmoUnitLength)),
		Vertex3D(Vector4(-Vector3::UnitZ * _GizmoWorldAxesLength)),
		Vertex3D(Vector4(Vector3::UnitZ * _GizmoWorldAxesLength)),
	};

	Matrix4x4 viewMat = _GameEngine3.GetMainCamera().GetViewMatrix();
	Matrix4x4 perspMat = _GameEngine3.GetMainCamera().GetPerspectiveMatrix(_ScreenSize.X, _ScreenSize.Y);
	Matrix4x4 finalMat = perspMat * viewMat;
	Apply3DTransform(gizmos, finalMat, _ScreenSize);

	static Vector4 screenOffset(-320.f, -250.f, 0.f, 0.f);

	// X축 그리기
	_RSI->DrawLine(gizmos[0].Position + screenOffset, gizmos[1].Position + screenOffset, LinearColor::Red);
	_RSI->DrawLine(gizmos[2].Position, gizmos[3].Position, LinearColor::Gray);

	// Y축 그리기
	_RSI->DrawLine(gizmos[0].Position + screenOffset, gizmos[4].Position + screenOffset, LinearColor::Green);
	_RSI->DrawLine(gizmos[5].Position, gizmos[6].Position, LinearColor::Gray);

	// Z축 그리기
	_RSI->DrawLine(gizmos[0].Position + screenOffset, gizmos[7].Position + screenOffset, LinearColor::Blue);
	_RSI->DrawLine(gizmos[8].Position, gizmos[9].Position, LinearColor::Gray);
}


// 게임 로직
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	static float rotateSpeed = 180.f;
	InputManager input = _GameEngine3.GetInputManager();

	Transform& playerTransform = _GameEngine3.FindGameObject(GameEngine::PlayerKey).GetTransform();
	playerTransform.AddYawRotation(input.GetXAxis() * rotateSpeed * InDeltaSeconds);
	playerTransform.AddPitchRotation(-input.GetYAxis() * rotateSpeed * InDeltaSeconds);
}

// 렌더링 로직
void SoftRenderer::Render3D()
{
	// 격자 그리기
	DrawGizmo3D();

	Matrix4x4 viewMat = _GameEngine3.GetMainCamera().GetViewMatrix();
	Matrix4x4 perspMat = _GameEngine3.GetMainCamera().GetPerspectiveMatrix(_ScreenSize.X, _ScreenSize.Y);
	const Texture& steveTexture = _GameEngine3.GetMainTexture();

	for (auto it = _GameEngine3.SceneBegin(); it != _GameEngine3.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *it;
		const Mesh& mesh = _GameEngine3.GetMesh(gameObject.GetMeshKey());
		const Transform& transform = gameObject.GetTransformConst();
		Matrix4x4 finalMat = perspMat * viewMat * transform.GetModelingMatrix();

		size_t vertexCount = mesh._Vertices.size();
		size_t indexCount = mesh._Indices.size();
		size_t triangleCount = indexCount / 3;

		// 렌더러가 사용할 정점 버퍼와 인덱스 버퍼 생성
		Vertex3D* vertices = new Vertex3D[vertexCount];
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi].Position = Vector4(mesh._Vertices[vi], true);
			vertices[vi].UV = mesh._UVs[vi];
		}

		int* indices = new int[indexCount];
		std::memcpy(indices, mesh._Indices.data(), sizeof(int) * indexCount);

		// 각 정점에 행렬을 적용

		//for (int vi = 0; vi < vertexCount; ++vi)
		//{
		//	vertices[vi].Position = finalMat * vertices[vi].Position;
		//}

		// 변환된 정점을 잇는 선 그리기
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi = ti * 3;
			std::vector<Vertex3D> tp = { vertices[indices[bi]] , vertices[indices[bi + 1]] , vertices[indices[bi + 2]] };
			Apply3DTransform(tp, finalMat, _ScreenSize);

			//for (Vertex3D& v : tp)
			//{
			//	float invW = 1.f / v.Position.W;
			//	v.Position.X *= invW;
			//	v.Position.Y *= invW;
			//	v.Position.Z *= invW;
			//	v.Position.W = invW;
			//}

			Vector3 edge1 = (tp[1].Position - tp[0].Position).ToVector3();
			Vector3 edge2 = (tp[2].Position - tp[0].Position).ToVector3();
			Vector3 faceNormal = edge1.Cross(edge2).Normalize();
			if (faceNormal.Dot(-Vector3::UnitZ) > 0.f)
			{
				continue;
			}

			//for (Vertex3D& v : tp)
			//{
			//	v.Position.X *= (_ScreenSize.X * 0.5f);
			//	v.Position.Y *= (_ScreenSize.Y * 0.5f);
			//}

			_RSI->DrawLine(tp[0].Position, tp[1].Position, gameObject.GetColor());
			_RSI->DrawLine(tp[0].Position, tp[2].Position, gameObject.GetColor());
			_RSI->DrawLine(tp[1].Position, tp[2].Position, gameObject.GetColor());
		}

		delete[] vertices;
		delete[] indices;
	}
}

