
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
	static float moveSpeed = 100.f;
	static float rotateSpeed = 180.f;

	InputManager input = _GameEngine3.GetInputManager();

	// 플레이어 게임 오브젝트 트랜스폼의 변경
	GameObject& player = _GameEngine3.FindGameObject(GameEngine::PlayerKey);
	if (!player.IsNotFound())
	{
		Transform& playerTransform = player.GetTransform();
		playerTransform.AddPosition(Vector3(input.GetXAxis(), input.GetYAxis(), input.GetZAxis()) * moveSpeed * InDeltaSeconds);
		playerTransform.AddPitchRotation(-input.GetWAxis() * rotateSpeed * InDeltaSeconds);
		_GameEngine3.GetMainCamera().SetLookAtRotation(player.GetTransform().GetPosition());
	}

	// 기즈모 토글
	if (input.SpacePressed()) { _Show3DGizmo = !_Show3DGizmo; }
}

// 렌더링 로직
void SoftRenderer::Render3D()
{
	// 기즈모 그리기
	if (_Show3DGizmo)
	{
		DrawGizmo3D();
	}

	const Camera& mainCamera = _GameEngine3.GetMainCamera();
	Matrix4x4 viewMat = mainCamera.GetViewMatrix();
	Matrix4x4 perspMat = mainCamera.GetPerspectiveMatrix();
	Matrix4x4 pvMat = perspMat * viewMat;
	ScreenPoint viewportSize = mainCamera.GetViewportSize();

	for (auto it = _GameEngine3.SceneBegin(); it != _GameEngine3.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *it;
		const Transform& transform = gameObject.GetTransformConst();

		// 물체가 카메라 뒤에 있으면 그리지 않도록 처리
		Vector3 viewPos = viewMat * transform.GetPosition();
		if (viewPos.Z >= 0.f)
		{
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

		// 클립 공간을 화면 공간으로 변환
		for (Vertex3D& v : vertices)
		{
			// NDC 공간으로 변환
			float invZ = 1.f / v.Position.Z;
			v.Position.X *= invZ;
			v.Position.Y *= invZ;

			// Z 값은 백페이스 컬링을 위해 원래대로 돌려둠
			v.Position.Z = -v.Position.Z;

			// 화면 공간으로 확장
			v.Position.X *= (viewportSize.X * 0.5f);
			v.Position.Y *= (viewportSize.Y * 0.5f);
		}

		// 변환된 정점을 잇는 선 그리기
		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
			Vertex3D& tv0 = vertices[indice[bi0]];
			Vertex3D& tv1 = vertices[indice[bi1]];
			Vertex3D& tv2 = vertices[indice[bi2]];

			// 백페이스 컬링 ( 뒷면이면 그리기 생략 )
			Vector3 edge1 = (tv1.Position - tv0.Position).ToVector3();
			Vector3 edge2 = (tv2.Position - tv0.Position).ToVector3();
			if (edge1.Cross(edge2).Z <= 0.f)
			{
				continue;
			}

			// 게임 오브젝트의 색상 결정
			LinearColor objectColor = FragmentShader3D(gameObject.GetColor());

			if (gameObject == GameEngine::PlayerKey)
			{
				// 와이어프레임 그리기
				_RSI->DrawLine(tv0.Position, tv1.Position, objectColor);
				_RSI->DrawLine(tv0.Position, tv2.Position, objectColor);
				_RSI->DrawLine(tv1.Position, tv2.Position, objectColor);
			}
			else
			{
				if (!_Show3DGizmo)
					continue;

				// 삼각형 칠하기
				// 삼각형의 영역 설정
				Vector2 minPos(Math::Min3(tv0.Position.X, tv1.Position.X, tv2.Position.X), Math::Min3(tv0.Position.Y, tv1.Position.Y, tv2.Position.Y));
				Vector2 maxPos(Math::Max3(tv0.Position.X, tv1.Position.X, tv2.Position.X), Math::Max3(tv0.Position.Y, tv1.Position.Y, tv2.Position.Y));

				// 무게중심좌표를 위해 점을 벡터로 변환
				Vector2 u = tv1.Position.ToVector2() - tv0.Position.ToVector2();
				Vector2 v = tv2.Position.ToVector2() - tv0.Position.ToVector2();

				// 공통 분모 값 ( uu * vv - uv * uv )
				float udotv = u.Dot(v);
				float vdotv = v.Dot(v);
				float udotu = u.Dot(u);
				float denominator = udotv * udotv - vdotv * udotu;

				// 퇴화 삼각형의 판정
				if (Math::EqualsInTolerance(denominator, 0.0f))
				{
					// 퇴화 삼각형이면 건너뜀.
					continue;
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
						Vector2 w = pointToTest - tv0.Position.ToVector2();
						float wdotu = w.Dot(u);
						float wdotv = w.Dot(v);

						float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
						float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
						float oneMinusST = 1.f - s - t;
						if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
						{
							_RSI->DrawPoint(fragment, objectColor);
						}
					}
				}
			}
		}
	}

	const GameObject& player = _GameEngine3.FindGameObject(GameEngine::PlayerKey);
	if (!player.IsNotFound())
	{
		const Transform& playerTransform = player.GetTransformConst();
		_RSI->PushStatisticText("Player Position : " + playerTransform.GetPosition().ToString());
	}
}

