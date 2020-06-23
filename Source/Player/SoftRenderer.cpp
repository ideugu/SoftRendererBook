
#include "Precompiled.h"
#include "SoftRenderer.h"

SoftRenderer::SoftRenderer(RenderingSoftwareInterface* InRSI) : _RSI(InRSI)
{
}

void SoftRenderer::OnTick()
{
	if (!_AllInitialized)
	{
		// �����ս� ī���� �ʱ�ȭ.
		if(PerformanceInitFunc && PerformanceMeasureFunc)
		{
			_CyclesPerMilliSeconds = PerformanceInitFunc();
			_PerformanceCheckInitialized = true;
		}
		else
		{
			return;
		}

		// ��ũ�� ũ�� Ȯ��
		if (_ScreenSize.HasZero())
		{
			return;
		}

		// ����Ʈ ������ �ʱ�ȭ.
		if (!_RSI->Init(_ScreenSize))
		{
			return;
		}

		_RendererInitialized = true;

		// ���� ���� �ʱ�ȭ
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

	// ũ�Ⱑ ����Ǹ� �������� ���� �ʱ�ȭ
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
	// ���� ���� ����.
	_FrameTimeStamp = PerformanceMeasureFunc();
	if (_FrameCount == 0)
	{
		_StartTimeStamp = _FrameTimeStamp;
	}

	// ��� �����.
	_RSI->Clear(LinearColor::White);
}

void SoftRenderer::PostUpdate()
{
	// ������ ���� ����.
	RenderFrame();

	// ������ ������.
	_RSI->EndFrame();

	// ���� ���� ������.
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


