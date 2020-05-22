#pragma once

#include <memory>
#include <functional>
#include "InputManager.h"

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
	// 성능 측정을 위한 함수
	std::function<float()> PerformanceInitFunc;
	std::function<INT64()> PerformanceMeasureFunc;

	// 틱 처리를 위한 함수
	std::function<void()> RenderFrameFunc;
	std::function<void(float DeltaSeconds)> UpdateFunc;

	// 게임 엔진 레퍼런스
	GameEngine& GetGameEngine() { return _GameEngine; }

private:
	// 기본 루프 함수
	void PreUpdate();
	void PostUpdate();
	void Update();
	void RenderFrame();

private:
	// 실습을 위한 주요 함수 연동
	void BindTickFunctions();

private:
	// 2D 구현 함수
	void Update2D(float InDeltaSeconds);
	void Render2D();
	void DrawGrid2D();

	int _Grid2DUnit = 10;

private:
	// 초기화 점검 변수
	bool _PerformanceCheckInitialized = false;
	bool _RendererInitialized = false;
	bool _GameEngineInitialized = false;
	bool _TickFunctionBound = false;
	bool _AllInitialized = false;

	// 화면 크기
	ScreenPoint _ScreenSize;

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
	GameEngine _GameEngine;
};
