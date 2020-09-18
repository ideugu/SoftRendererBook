
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

// 정점의 선언
struct Vertex2D
{
public:
	Vertex2D() = default;
	Vertex2D(const Vector2& InPosition, const Vector2& InUV) : Position(InPosition), UV(InUV) { }

	Vector2 Position;
	Vector2 UV;
};

// 그리드 그리기
void SoftRenderer::DrawGrid2D()
{
	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 뷰의 영역 계산
	Vector2 viewPos = _GameEngine.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// 좌측 하단에서부터 격자 그리기
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// 그리드가 시작되는 좌하단 좌표 값 계산
	Vector2 minPos = viewPos - extent;
	Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
	ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

	for (int ix = 0; ix < xGridCount; ++ix)
	{
		_RSI->DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy)
	{
		_RSI->DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	// 월드의 원점
	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	_RSI->DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	_RSI->DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}


// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	static float moveSpeed = 300.f;
	static float rotateSpeed = 180.f;
	static float scaleSpeed = 180.f;

	InputManager input = _GameEngine.GetInputManager();

	// 플레이어 게임 오브젝트 트랜스폼의 변경
	GameObject& player = _GameEngine.FindGameObject(GameEngine::PlayerKey);
	if (!player.IsNotFound())
	{
		Transform& playerTransform = player.GetTransform();
		playerTransform.AddPosition(Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds);
		playerTransform.AddRotation(input.GetWAxis() * rotateSpeed * InDeltaSeconds);
		float newScale = Math::Clamp(playerTransform.GetScale().X + scaleSpeed * input.GetZAxis() * InDeltaSeconds, 15.f, 30.f);
		playerTransform.SetScale(Vector2::One * newScale);

		// 플레이어를 따라다니는 카메라의 트랜스폼
		static float thresholdDistance = 1.f;
		Transform& cameraTransform = _GameEngine.GetMainCamera().GetTransform();
		Vector2 playerPosition = playerTransform.GetPosition();
		Vector2 prevCameraPosition = cameraTransform.GetPosition();
		if ((playerPosition - prevCameraPosition).SizeSquared() < thresholdDistance * thresholdDistance)
		{
			cameraTransform.SetPosition(playerPosition);
		}
		else
		{
			static float lerpSpeed = 2.f;
			float ratio = lerpSpeed * InDeltaSeconds;
			ratio = Math::Clamp(ratio, 0.f, 1.f);
			Vector2 newCameraPosition = prevCameraPosition + (playerPosition - prevCameraPosition) * ratio;
			cameraTransform.SetPosition(newCameraPosition);
		}
	}
}

// 정점 변환 코드
FORCEINLINE void VertexShader2D(std::vector<Vertex2D>& InVertices, Matrix3x3 InMatrix)
{
	// 위치 값에 최종 행렬을 적용해 변환
	for (Vertex2D& v : InVertices)
	{
		v.Position = InMatrix * v.Position;
	}
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	// 전체 그릴 물체의 수
	size_t totalObjectCount = _GameEngine.GetScene().size();

	// 카메라의 뷰 행렬
	Matrix3x3 viewMat = _GameEngine.GetMainCamera().GetViewMatrix();

	// 랜덤하게 생성된 모든 게임 오브젝트들
	for (auto it = _GameEngine.SceneBegin(); it != _GameEngine.SceneEnd(); ++it)
	{
		// 게임 오브젝트에 필요한 내부 정보를 가져오기
		const GameObject& gameObject = *it;
		if (!gameObject.HasMesh())
		{
			continue;
		}

		const Mesh& mesh = _GameEngine.GetMesh(gameObject.GetMeshKey());
		const Transform& transform = gameObject.GetTransformConst();
		Matrix3x3 finalMat = viewMat * transform.GetModelingMatrix();

		size_t vertexCount = mesh._Vertices.size();
		size_t indexCount = mesh._Indices.size();
		size_t triangleCount = indexCount / 3;

		// 렌더러가 사용할 정점 버퍼와 인덱스 버퍼로 변환
		std::vector<Vertex2D> vertices(vertexCount);
		std::vector<int> indice(mesh._Indices);
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi].Position = mesh._Vertices[vi];
			vertices[vi].UV = mesh._UVs[vi];
		}

		// 정점 변환 진행
		VertexShader2D(vertices, finalMat);

		for (int ti = 0; ti < triangleCount; ++ti)
		{
			int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
			Vertex2D& tv0 = vertices[indice[bi0]];
			Vertex2D& tv1 = vertices[indice[bi1]];
			Vertex2D& tv2 = vertices[indice[bi2]];

			if (gameObject != GameEngine::PlayerKey)
			{
				_RSI->DrawLine(tv0.Position, tv1.Position, gameObject.GetColor());
				_RSI->DrawLine(tv0.Position, tv2.Position, gameObject.GetColor());
				_RSI->DrawLine(tv1.Position, tv2.Position, gameObject.GetColor());
			}
			else
			{
				Vector2 minPos(Math::Min3(tv0.Position.X, tv1.Position.X, tv2.Position.X), Math::Min3(tv0.Position.Y, tv1.Position.Y, tv2.Position.Y));
				Vector2 maxPos(Math::Max3(tv0.Position.X, tv1.Position.X, tv2.Position.X), Math::Max3(tv0.Position.Y, tv1.Position.Y, tv2.Position.Y));

				// 무게중심좌표를 위해 점을 벡터로 변환
				Vector2 u = tv1.Position - tv0.Position;
				Vector2 v = tv2.Position - tv0.Position;

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

				// 삼각형 영역 내 모든 점을 점검하고 색칠
				for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
				{
					for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
					{
						ScreenPoint fragment = ScreenPoint(x, y);
						Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
						Vector2 w = pointToTest - tv0.Position;
						float wdotu = w.Dot(u);
						float wdotv = w.Dot(v);

						float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
						float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
						float oneMinusST = 1.f - s - t;
						if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
						{
							Vector2 outUV = tv0.UV * oneMinusST + tv1.UV * s + tv2.UV * t;
							_RSI->DrawPoint(fragment, _GameEngine.GetMainTexture().GetColor(outUV));
						}
					}
				}
			}
		}
	}

	_RSI->PushStatisticText("Total Game Objects : " + std::to_string(totalObjectCount));

	const GameObject& player = _GameEngine.FindGameObject(GameEngine::PlayerKey);
	if (!player.IsNotFound())
	{
		const Transform& playerTransform = player.GetTransformConst();
		_RSI->PushStatisticText("Player Position : " + playerTransform.GetPosition().ToString());
		_RSI->PushStatisticText("Player Rotation : " + std::to_string(playerTransform.GetRotation()) + " (deg)");
		_RSI->PushStatisticText("Player Scale : " + std::to_string(playerTransform.GetScale().X));
	}
	
}

