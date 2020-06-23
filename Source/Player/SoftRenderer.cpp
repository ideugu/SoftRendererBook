
#include "Precompiled.h"
#include "SoftRenderer.h"

SoftRenderer::SoftRenderer(RenderingSoftwareInterface* InRSI) : _RSI(InRSI)
{
}

void SoftRenderer::OnTick()
{
	if (!_AllInitialized)
	{
		// 퍼포먼스 카운터 초기화.
		if(PerformanceInitFunc && PerformanceMeasureFunc)
		{
			_CyclesPerMilliSeconds = PerformanceInitFunc();
			_PerformanceCheckInitialized = true;
		}
		else
		{
			return;
		}

		// 스크린 크기 확인
		if (_ScreenSize.HasZero())
		{
			return;
		}

		// 소프트 렌더러 초기화.
		if (!_RSI->Init(_ScreenSize))
		{
			return;
		}

		_RendererInitialized = true;

		// 게임 엔진 초기화
		if (!_GameEngine.Init(_ScreenSize))
		{
			return;
		}

		_GameEngineInitialized = true;

		_AllInitialized = _RendererInitialized && _PerformanceCheckInitialized && _GameEngineInitialized;
		if (_AllInitialized)
		{
			BindTickFunctions();
		}
	}
	else
	{
		assert(_RSI != nullptr && _RSI->IsInitialized() && !_ScreenSize.HasZero());

		PreUpdate();
		Update();
		PostUpdate();
	}
}

void SoftRenderer::OnResize(const ScreenPoint& InNewScreenSize)
{
	_ScreenSize = InNewScreenSize;

	// 크기가 변경되면 렌더러와 엔진 초기화
	if (_RendererInitialized)
	{
		_RSI->Init(InNewScreenSize);
	}

	if (_GameEngineInitialized)
	{
		_GameEngine.Init(_ScreenSize);
	}
	
}

void SoftRenderer::OnShutdown()
{
	_RSI->Shutdown();
}

void SoftRenderer::PreUpdate()
{
	// 성능 측정 시작.
	_FrameTimeStamp = PerformanceMeasureFunc();
	if (_FrameCount == 0)
	{
		_StartTimeStamp = _FrameTimeStamp;
	}

	// 배경 지우기.
	_RSI->Clear(LinearColor::White);
}

void SoftRenderer::PostUpdate()
{
	// 렌더링 로직 수행.
	RenderFrame();

	// 렌더링 마무리.
	_RSI->EndFrame();

	// 성능 측정 마무리.
	_FrameCount++;
	INT64 currentTimeStamp = PerformanceMeasureFunc();
	INT64 frameCycles = currentTimeStamp - _FrameTimeStamp;
	INT64 elapsedCycles = currentTimeStamp - _StartTimeStamp;
	_FrameTime = frameCycles / _CyclesPerMilliSeconds;
	_ElapsedTime = elapsedCycles / _CyclesPerMilliSeconds;
	_FrameFPS = _FrameTime == 0.f ? 0.f : 1000.f / _FrameTime;
	_AverageFPS = _ElapsedTime == 0.f ? 0.f : 1000.f / _ElapsedTime * _FrameCount;
}

void SoftRenderer::RenderFrame()
{
	if (_TickFunctionBound)
	{
		RenderFrameFunc();
	}
}

void SoftRenderer::Update()
{
	if (_TickFunctionBound)
	{
		UpdateFunc(_FrameTime / 1000.f);
	}
}

void SoftRenderer::BindTickFunctions()
{
	using namespace std::placeholders;
	RenderFrameFunc = std::bind(&SoftRenderer::Render2D, this);
	UpdateFunc = std::bind(&SoftRenderer::Update2D, this, _1);
	_TickFunctionBound = true;
}


