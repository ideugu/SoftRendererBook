
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DDD;

// 그리드 그리기
void SoftRenderer::DrawGizmo3D()
{
}


// 게임 로직
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	static float rotateSpeed = 180.f;
	InputManager input = _GameEngine3.GetInputManager();

	Transform& playerTransform = _GameEngine3.GetPlayer()->GetTransform();
	playerTransform.AddPitchRotation(input.GetXAxis() * rotateSpeed * InDeltaSeconds);
}

// 렌더링 로직
void SoftRenderer::Render3D()
{
	// 격자 그리기
	DrawGizmo3D();

	Matrix4x4 viewMat = _GameEngine3.GetCamera()->GetViewMatrix();
	Matrix4x4 perspMat = _GameEngine3.GetCamera()->GetPerspectiveMatrix(_ScreenSize.X, _ScreenSize.Y);

	for (auto it = _GameEngine3.GoBegin(); it != _GameEngine3.GoEnd(); ++it)
	{
		GameObject* gameObject = it->get();
		const Mesh* mesh = _GameEngine3.GetMesh(gameObject->GetMeshKey());
		Transform& transform = gameObject->GetTransform();
		Matrix4x4 finalMat = perspMat * viewMat * transform.GetModelingMatrix();

		size_t vertexCount = mesh->_Vertices.size();
		size_t indexCount = mesh->_Indices.size();
		size_t triangleCount = indexCount / 3;

		// 렌더러가 사용할 정점 버퍼와 인덱스 버퍼 생성
		Vector4* vertices = new Vector4[vertexCount];
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi] = Vector4(mesh->_Vertices[vi], true);
		}

		int* indices = new int[indexCount];
		std::memcpy(indices, mesh->_Indices.data(), sizeof(int) * indexCount);

		// 각 정점에 행렬을 적용
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi] = finalMat * vertices[vi];
		}

		// 변환된 정점을 잇는 선 그리기
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi = ti * 3;
			std::vector<Vector4> tp = { vertices[indices[bi]] , vertices[indices[bi + 1]] , vertices[indices[bi + 2]] };

			for(Vector4 & v : tp)
			{
				float invW = 1.f / v.W;
				v.X *= invW;
				v.Y *= invW;
				v.Z *= invW;
				v.W = invW;
			}

			Vector3 edge1 = (tp[1] - tp[0]).ToVector3();
			Vector3 edge2 = (tp[2] - tp[0]).ToVector3();
			Vector3 faceNormal = edge1.Cross(edge2).Normalize();
			if (faceNormal.Dot(-Vector3::UnitZ) > 0.f)
			{
				continue;
			}

			for (Vector4& v : tp)
			{
				//v.Z = (v.Z + 1.f) * 0.5f;
				v.X *= (_ScreenSize.X * 0.5f);
				v.Y *= (_ScreenSize.Y * 0.5f);
			}

			_RSI->DrawLine(tp[0].ToVector2(), tp[1].ToVector2(), gameObject->GetColor());
			_RSI->DrawLine(tp[0].ToVector2(), tp[2].ToVector2(), gameObject->GetColor());
			_RSI->DrawLine(tp[1].ToVector2(), tp[2].ToVector2(), gameObject->GetColor());
		}

		delete[] vertices;
		delete[] indices;
	}
}

