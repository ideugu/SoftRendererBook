
#include "Precompiled.h"
#include "SoftRenderer.h"

SoftRenderer::SoftRenderer(RenderingSoftwareInterface* InRSI) : _RSIPtr(InRSI)
{
}

void SoftRenderer::OnTick()
{
	if (!_AllInitialized)
	{
		// 퍼포먼스 카운터 초기화.
		if(_PerformanceInitFunc && _PerformanceMeasureFunc)
		{
			_CyclesPerMilliSeconds = _PerformanceInitFunc();
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
		if (!GetRenderer().Init(_ScreenSize))
		{
			return;
		}

		_RendererInitialized = true;

		// 게임 엔진 초기화
		Get3DGameEngine().OnScreenResize(_ScreenSize);
		if (!Get3DGameEngine().Init())
		{
			return;
		}

		_GameEngineInitialized = true;

		_AllInitialized = _RendererInitialized && _PerformanceCheckInitialized && _GameEngineInitialized;
		if (_AllInitialized)
		{
			_TickEnabled = true;
		}
	}
	else
	{
		assert(_RSIPtr != nullptr && GetRenderer().IsInitialized() && !_ScreenSize.HasZero());

		if (_TickEnabled)
		{
			PreUpdate();

			// 게임 로직 수행.
			Update3D(_FrameTime / 1000.f);

			// 애니메이션 로직 수행.
			LateUpdate3D(_FrameTime / 1000.f);

			// 렌더링 로직 수행.
			Render3D();

			PostUpdate();
		}
	}
}

void SoftRenderer::OnResize(const ScreenPoint& InNewScreenSize)
{
	_ScreenSize = InNewScreenSize;

	// 크기가 변경되면 렌더러와 엔진 초기화
	if (_RendererInitialized)
	{
		GetRenderer().Init(InNewScreenSize);
	}

	if (_GameEngineInitialized)
	{
		Get3DGameEngine().OnScreenResize(_ScreenSize);
	}	
}

void SoftRenderer::OnShutdown()
{
	GetRenderer().Shutdown();
}

void SoftRenderer::PreUpdate()
{
	// 성능 측정 시작.
	_FrameTimeStamp = _PerformanceMeasureFunc();
	if (_FrameCount == 0)
	{
		_StartTimeStamp = _FrameTimeStamp;
	}

	// 배경 지우기.
	GetRenderer().Clear(_BackgroundColor);

	// 버퍼 시각화
	const InputManager& input = Get3DGameEngine().GetInputManager();

	if (input.IsReleased(InputButton::F1)) { _CurrentDrawMode = DrawMode::Normal; }
	if (input.IsReleased(InputButton::F2)) { _CurrentDrawMode = DrawMode::Wireframe; }
	if (input.IsReleased(InputButton::F3)) { _CurrentDrawMode = DrawMode::DepthBuffer; }
}

void SoftRenderer::PostUpdate()
{
	// 렌더링 마무리.
	GetRenderer().EndFrame();

	// 입력 상태 업데이트
	Get3DGameEngine().GetInputManager().UpdateInput();

	// 성능 측정 마무리.
	_FrameCount++;
	INT64 currentTimeStamp = _PerformanceMeasureFunc();
	INT64 frameCycles = currentTimeStamp - _FrameTimeStamp;
	INT64 elapsedCycles = currentTimeStamp - _StartTimeStamp;
	_FrameTime = frameCycles / _CyclesPerMilliSeconds;
	_ElapsedTime = elapsedCycles / _CyclesPerMilliSeconds;
	_FrameFPS = _FrameTime == 0.f ? 0.f : 1000.f / _FrameTime;
	_AverageFPS = _ElapsedTime == 0.f ? 0.f : 1000.f / _ElapsedTime * _FrameCount;
}


