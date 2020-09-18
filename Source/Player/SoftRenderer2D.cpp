
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

struct Vertex2D
{
public:
	Vertex2D() = default;
	Vertex2D(const Vector2& InPosition, const Vector2& InTextureCoordinate) : Position(InPosition), UV(InTextureCoordinate) { }

	Vector2 Position;
	Vector2 UV;
};


// 그리드 그리기
void SoftRenderer::DrawGrid2D()
{
	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 가로 세로 라인 그리기
	ScreenPoint screenHalfSize = _ScreenSize.GetHalf();

	for (int x = screenHalfSize.X; x <= _ScreenSize.X; x += _Grid2DUnit)
	{
		_RSI->DrawFullVerticalLine(x, gridColor);
		if (x > screenHalfSize.X)
		{
			_RSI->DrawFullVerticalLine(2 * screenHalfSize.X - x, gridColor);
		}
	}

	for (int y = screenHalfSize.Y; y <= _ScreenSize.Y; y += _Grid2DUnit)
	{
		_RSI->DrawFullHorizontalLine(y, gridColor);
		if (y > screenHalfSize.Y)
		{
			_RSI->DrawFullHorizontalLine(2 * screenHalfSize.Y - y, gridColor);
		}
	}

	// 월드 축 그리기
	_RSI->DrawFullHorizontalLine(screenHalfSize.Y, LinearColor::Red);
	_RSI->DrawFullVerticalLine(screenHalfSize.X, LinearColor::Green);
}


// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	static float moveSpeed = 100.f;
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
	}
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	// 전체 그릴 물체의 수
	size_t totalObjectCount = _GameEngine.GetScene().size();

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
		Matrix3x3 finalMat = transform.GetModelingMatrix();

		size_t vertexCount = mesh._Vertices.size();
		size_t indexCount = mesh._Indices.size();
		size_t triangleCount = indexCount / 3;

		// 렌더러가 사용할 정점 버퍼와 인덱스 버퍼 생성
		Vertex2D* vertices = new Vertex2D[vertexCount];
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi].Position = mesh._Vertices[vi];
			vertices[vi].UV = mesh._UVs[vi];
		}

		int* indices = new int[indexCount];
		std::memcpy(indices, &mesh._Indices[0], sizeof(int) * indexCount);

		// 각 정점에 행렬을 적용
		for (int vi = 0; vi < vertexCount; ++vi)
		{
			vertices[vi].Position = finalMat * vertices[vi].Position;
		}

		if (gameObject != GameEngine::PlayerKey)
		{
			// 변환된 정점을 잇는 선 그리기
			for (int ti = 0; ti < triangleCount; ++ti)
			{
				int bi = ti * 3;
				_RSI->DrawLine(vertices[indices[bi]].Position, vertices[indices[bi + 1]].Position, gameObject.GetColor());
				_RSI->DrawLine(vertices[indices[bi]].Position, vertices[indices[bi + 2]].Position, gameObject.GetColor());
				_RSI->DrawLine(vertices[indices[bi + 1]].Position, vertices[indices[bi + 2]].Position, gameObject.GetColor());
			}
		}
		else
		{
			for (int ti = 0; ti < triangleCount; ++ti)
			{
				// 삼각형마다 칠하기
				int bi = ti * 3;
				std::vector<Vertex2D> tv = { vertices[indices[bi]] , vertices[indices[bi + 1]], vertices[indices[bi + 2]] };

				Vector2 minPos(Math::Min3(tv[0].Position.X, tv[1].Position.X, tv[2].Position.X), Math::Min3(tv[0].Position.Y, tv[1].Position.Y, tv[2].Position.Y));
				Vector2 maxPos(Math::Max3(tv[0].Position.X, tv[1].Position.X, tv[2].Position.X), Math::Max3(tv[0].Position.Y, tv[1].Position.Y, tv[2].Position.Y));

				// 무게중심좌표를 위한 준비작업
				Vector2 u = tv[1].Position - tv[0].Position;
				Vector2 v = tv[2].Position - tv[0].Position;

				// 공통 분모 값을 구할 것. ( uu * vv - uv * uv )
				float udotv = u.Dot(v);
				float vdotv = v.Dot(v);
				float udotu = u.Dot(u);
				float denominator = udotv * udotv - vdotv * udotu;
				if (Math::EqualsInTolerance(denominator, 0.0f))
				{
					continue;
				}
				float invDenominator = 1.f / denominator;

				// 화면상의 점 구하기
				ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
				ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

				// 모든 점을 Loop
				for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
				{
					for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
					{
						ScreenPoint fragment = ScreenPoint(x, y);
						Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
						Vector2 w = pointToTest - tv[0].Position;
						float wdotu = w.Dot(u);
						float wdotv = w.Dot(v);

						float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
						float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
						float oneMinusST = 1.f - s - t;
						if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
						{
							Vector2 outUV = tv[0].UV * oneMinusST + tv[1].UV * s + tv[2].UV * t;
							_RSI->DrawPoint(fragment, _GameEngine.GetMainTexture().GetColor(outUV));
						}
					}
				}
			}
		}

		delete[] vertices;
		delete[] indices;
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

