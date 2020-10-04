#pragma once

class SoftRenderer
{
public:
	// 생성자
	SoftRenderer(RenderingSoftwareInterface* InRSI);

public:
	// 윈도우 이벤트 처리 함수
	void OnTick();
	void OnResize(const ScreenPoint& InNewScreenSize);
	void OnShutdown();

public:
	// 프로그램 기본 정보
	const ScreenPoint& GetScreenSize() { return _ScreenSize; }
	float GetFrameFPS() const { return _FrameFPS; }
	float GetElapsedTime() const { return _ElapsedTime; }

public:
	// 성능 측정을 위한 함수  ( 외부 연동용 ) 
	std::function<float()> _PerformanceInitFunc;
	std::function<INT64()> _PerformanceMeasureFunc;

	// 게임 엔진 레퍼런스 ( 외부 연동 및 공용 로직 ) 
	FORCEINLINE DDD::GameEngine& GetGameEngine() { return _GameEngine3; }
	FORCEINLINE const DDD::GameEngine& GetGameEngineC() { return _GameEngine3; }

private:
	// 기본 루프 함수
	void PreUpdate();
	void PostUpdate();

private:

	enum class ShowMode : UINT32
	{
		Normal = 0,
		Wireframe,
		DepthBuffer
	};

	// 2D 구현 함수
	void Update2D(float InDeltaSeconds);
	void Render2D();
	void DrawGrid2D();

	int _Grid2DUnit = 10;

	// 3D 구현함수
	void DrawGizmo3D();
	void Update3D(float InDeltaSeconds);
	void Render3D();

	void DrawTriangle(std::vector<struct Vertex3D>& vertices, const LinearColor& InColorParam);

	float _GizmoUnitLength = 50.f;
	Vector2 _GizmoPositionOffset = Vector2(-320.f, -250.f);

	ShowMode _CurrentShowMode = ShowMode::Normal;

private:
	// 초기화 점검 변수
	bool _PerformanceCheckInitialized = false;
	bool _RendererInitialized = false;
	bool _GameEngineInitialized = false;
	bool _TickEnabled = false;
	bool _AllInitialized = false;

	// 화면 크기
	ScreenPoint _ScreenSize;

	// 배경 색상
	LinearColor _BackgroundColor = LinearColor::White;

	// 성능 측정 관련 변수들
	long long _StartTimeStamp = 0;
	long long _FrameTimeStamp = 0;
	long _FrameCount = 0;
	float _CyclesPerMilliSeconds = 0.f;
	float _FrameTime = 0.f;
	float _ElapsedTime = 0.f;
	float _AverageFPS = 0.f;
	float _FrameFPS = 0.f;

	// 렌더러 인터페이스
	std::unique_ptr<RenderingSoftwareInterface> _RSI;

	// 게임 엔진
	DD::GameEngine _GameEngine2;
	DDD::GameEngine _GameEngine3;
};
