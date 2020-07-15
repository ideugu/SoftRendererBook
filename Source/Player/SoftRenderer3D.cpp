
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DDD;

struct Vertex
{
public:
	FORCEINLINE Vertex() = default;
	FORCEINLINE explicit Vertex(const Vector4& InPosition) : Position(InPosition) { }
	FORCEINLINE explicit Vertex(const Vector4& InPosition, const LinearColor& InColor) : Position(InPosition), Color(InColor) { }
	FORCEINLINE explicit Vertex(const Vector4& InPosition, const LinearColor& InColor, const Vector2& InTextureCoordinate) : Position(InPosition), Color(InColor), TextureCoordinate(InTextureCoordinate) { }

	Vector4 Position = Vector4::Zero;
	LinearColor Color = LinearColor::Error;
	Vector2 TextureCoordinate = Vector2::Zero;
};


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
	playerTransform.AddYawRotation(input.GetXAxis() * rotateSpeed * InDeltaSeconds);
	playerTransform.AddPitchRotation(-input.GetYAxis() * rotateSpeed * InDeltaSeconds);
}

// 렌더링 로직
void SoftRenderer::Render3D()
{
	// 격자 그리기
	DrawGizmo3D();

	Matrix4x4 viewMat = _GameEngine3.GetCamera()->GetViewMatrix();
	Matrix4x4 perspMat = _GameEngine3.GetCamera()->GetPerspectiveMatrix(_ScreenSize.X, _ScreenSize.Y);
	const Texture* steveTexture = _GameEngine3.GetTexture(GameEngine::SteveTextureKey);

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
		Vertex* vertices = new Vertex[vertexCount];
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi].Position = Vector4(mesh->_Vertices[vi], true);
			vertices[vi].TextureCoordinate = mesh->_UVs[vi];
		}

		int* indices = new int[indexCount];
		std::memcpy(indices, mesh->_Indices.data(), sizeof(int) * indexCount);

		// 각 정점에 행렬을 적용
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi].Position = finalMat * vertices[vi].Position;
		}

		// 변환된 정점을 잇는 선 그리기
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi = ti * 3;
			std::vector<Vertex> tp = { vertices[indices[bi]] , vertices[indices[bi + 1]] , vertices[indices[bi + 2]] };

			for(Vertex& v : tp)
			{
				float invW = 1.f / v.Position.W;
				v.Position.X *= invW;
				v.Position.Y *= invW;
				v.Position.Z *= invW;
				v.Position.W = invW;
			}

			Vector3 edge1 = (tp[1].Position - tp[0].Position).ToVector3();
			Vector3 edge2 = (tp[2].Position - tp[0].Position).ToVector3();
			Vector3 faceNormal = edge1.Cross(edge2).Normalize();
			if (faceNormal.Dot(-Vector3::UnitZ) > 0.f)
			{
				continue;
			}

			for (Vertex& v : tp)
			{
				//v.Z = (v.Z + 1.f) * 0.5f;
				v.Position.X *= (_ScreenSize.X * 0.5f);
				v.Position.Y *= (_ScreenSize.Y * 0.5f);
			}

			Vector2 minPos(INFINITY, INFINITY);
			Vector2 maxPos(-INFINITY, -INFINITY);
			for (Vertex& v : tp)
			{
				minPos.X = Math::Min(minPos.X, v.Position.X);
				minPos.Y = Math::Min(minPos.Y, v.Position.Y);

				maxPos.X = Math::Max(maxPos.X, v.Position.X);
				maxPos.Y = Math::Max(maxPos.Y, v.Position.Y);
			}

			Vector2 u = tp[1].Position.ToVector2() - tp[0].Position.ToVector2();
			Vector2 v = tp[2].Position.ToVector2() - tp[0].Position.ToVector2();
			float dotUU = u.Dot(u);
			float dotUV = u.Dot(v);
			float dotVV = v.Dot(v);
			float denominator = (dotUU * dotVV - dotUV * dotUV);
			if (denominator == 0.f)
			{
				continue;
			}
			float invDenominator = 1.f / (dotUU * dotVV - dotUV * dotUV);
			ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
			ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

			float invZ0 = tp[0].Position.W;
			float invZ1 = tp[1].Position.W;
			float invZ2 = tp[2].Position.W;

			for (int x = lowerLeftPoint.X; x <= (upperRightPoint.X + 1); ++x)
			{
				for (int y = upperRightPoint.Y; y <= (lowerLeftPoint.Y + 1); ++y)
				{
					ScreenPoint fragment = ScreenPoint(x, y);
					Vector2 pointToTest = fragment.ToVectorCoordinate(_ScreenSize);
					Vector2 w = pointToTest - tp[0].Position.ToVector2();
					float dotWU = w.Dot(u);
					float dotWV = w.Dot(v);
					float numeratorS = dotWU * dotVV - dotWV * dotUV;
					float numeratorT = dotWV * dotUU - dotWU * dotUV;

					float s = numeratorS * invDenominator;
					float t = numeratorT * invDenominator;
					float oneMinusST = 1.f - s - t;
					if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
					{
						float z = invZ0 * oneMinusST + invZ1 * s + invZ2 * t;
						float invZ = 1.f / z;
						Vector4 pixelPosition = (tp[0].Position * oneMinusST * invZ0 + tp[1].Position * s * invZ1 + tp[2].Position * t * invZ2) * invZ;
						Vector2 perPixelUV = (tp[0].TextureCoordinate * oneMinusST * invZ0 + tp[1].TextureCoordinate * s * invZ1 + tp[2].TextureCoordinate * t * invZ2) * invZ;

						int textureWidth = steveTexture->GetWidth();
						int textureHeight = steveTexture->GetHeight();
						int x = Math::FloorToInt(perPixelUV.X * textureWidth);
						int y = Math::FloorToInt(perPixelUV.Y * textureHeight);
						x %= textureWidth;
						y %= textureHeight;

						LinearColor finalColor = steveTexture->GetBuffer()[textureWidth * (textureHeight - (1 + y)) + x];

						_RSI->DrawPoint(fragment, finalColor);
					}
				}
			}
		}

		delete[] vertices;
		delete[] indices;
	}
}

