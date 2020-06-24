
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DDD;

// �׸��� �׸���
void SoftRenderer::DrawGizmo3D()
{
}


// ���� ����
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	static float rotateSpeed = 180.f;
	InputManager input = _GameEngine3.GetInputManager();

	Transform& playerTransform = _GameEngine3.GetPlayer()->GetTransform();
	playerTransform.AddPitchRotation(input.GetXAxis() * rotateSpeed * InDeltaSeconds);
}

// ������ ����
void SoftRenderer::Render3D()
{
	// ���� �׸���
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

		// �������� ����� ���� ���ۿ� �ε��� ���� ����
		Vector4* vertices = new Vector4[vertexCount];
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi] = Vector4(mesh->_Vertices[vi], true);
		}

		int* indices = new int[indexCount];
		std::memcpy(indices, mesh->_Indices.data(), sizeof(int) * indexCount);

		// �� ������ ����� ����
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi] = finalMat * vertices[vi];
		}

		// ��ȯ�� ������ �մ� �� �׸���
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

