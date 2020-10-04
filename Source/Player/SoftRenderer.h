#pragma once

class SoftRenderer
{
public:
	// ������
	SoftRenderer(RenderingSoftwareInterface* InRSI);

public:
	// ������ �̺�Ʈ ó�� �Լ�
	void OnTick();
	void OnResize(const ScreenPoint& InNewScreenSize);
	void OnShutdown();

public:
	// ���α׷� �⺻ ����
	const ScreenPoint& GetScreenSize() { return _ScreenSize; }
	float GetFrameFPS() const { return _FrameFPS; }
	float GetElapsedTime() const { return _ElapsedTime; }

public:
	// ���� ������ ���� �Լ�  ( �ܺ� ������ ) 
	std::function<float()> _PerformanceInitFunc;
	std::function<INT64()> _PerformanceMeasureFunc;

	// ���� ���� ���۷��� ( �ܺ� ���� �� ���� ���� ) 
	FORCEINLINE DDD::GameEngine& GetGameEngine() { return _GameEngine3; }
	FORCEINLINE const DDD::GameEngine& GetGameEngineC() { return _GameEngine3; }

private:
	// �⺻ ���� �Լ�
	void PreUpdate();
	void PostUpdate();

private:

	enum class ShowMode : UINT32
	{
		Normal = 0,
		Wireframe,
		DepthBuffer
	};

	// 2D ���� �Լ�
	void Update2D(float InDeltaSeconds);
	void Render2D();
	void DrawGrid2D();

	int _Grid2DUnit = 10;

	// 3D �����Լ�
	void DrawGizmo3D();
	void Update3D(float InDeltaSeconds);
	void Render3D();

	void DrawTriangle(std::vector<struct Vertex3D>& vertices, const LinearColor& InColorParam);

	float _GizmoUnitLength = 50.f;
	Vector2 _GizmoPositionOffset = Vector2(-320.f, -250.f);

	ShowMode _CurrentShowMode = ShowMode::Normal;

private:
	// �ʱ�ȭ ���� ����
	bool _PerformanceCheckInitialized = false;
	bool _RendererInitialized = false;
	bool _GameEngineInitialized = false;
	bool _TickEnabled = false;
	bool _AllInitialized = false;

	// ȭ�� ũ��
	ScreenPoint _ScreenSize;

	// ��� ����
	LinearColor _BackgroundColor = LinearColor::White;

	// ���� ���� ���� ������
	long long _StartTimeStamp = 0;
	long long _FrameTimeStamp = 0;
	long _FrameCount = 0;
	float _CyclesPerMilliSeconds = 0.f;
	float _FrameTime = 0.f;
	float _ElapsedTime = 0.f;
	float _AverageFPS = 0.f;
	float _FrameFPS = 0.f;

	// ������ �������̽�
	std::unique_ptr<RenderingSoftwareInterface> _RSI;

	// ���� ����
	DD::GameEngine _GameEngine2;
	DDD::GameEngine _GameEngine3;
};
