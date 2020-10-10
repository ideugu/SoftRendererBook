
#include "Precompiled.h"
#include "SoftRenderer.h"

SoftRenderer::SoftRenderer(RenderingSoftwareInterface* InRSI) : _RSIPtr(InRSI)
{
}

void SoftRenderer::OnTick()
{
	if (!_AllInitialized)
	{
		// �����ս� ī���� �ʱ�ȭ.
		if(_PerformanceInitFunc && _PerformanceMeasureFunc)
		{
			_CyclesPerMilliSeconds = _PerformanceInitFunc();
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
		if (!GetRSI().Init(_ScreenSize))
		{
			return;
		}

		_RendererInitialized = true;

		// ���� ���� �ʱ�ȭ
		GetGameEngine().OnScreenResize(_ScreenSize);
		if (!GetGameEngine().Init())
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
		assert(_RSIPtr != nullptr && GetRSI().IsInitialized() && !_ScreenSize.HasZero());

		if (_TickEnabled)
		{
			PreUpdate();

			// ���� ���� ����.
			Update3D(_FrameTime / 1000.f);

			// �ִϸ��̼� ���� ����.
			LateUpdate3D(_FrameTime / 1000.f);

			// ������ ���� ����.
			Render3D();

			PostUpdate();
		}
	}
}

void SoftRenderer::OnResize(const ScreenPoint& InNewScreenSize)
{
	_ScreenSize = InNewScreenSize;

	// ũ�Ⱑ ����Ǹ� �������� ���� �ʱ�ȭ
	if (_RendererInitialized)
	{
		GetRSI().Init(InNewScreenSize);
	}

	if (_GameEngineInitialized)
	{
		GetGameEngine().OnScreenResize(_ScreenSize);
	}	
}

void SoftRenderer::OnShutdown()
{
	GetRSI().Shutdown();
}

void SoftRenderer::PreUpdate()
{
	// ���� ���� ����.
	_FrameTimeStamp = _PerformanceMeasureFunc();
	if (_FrameCount == 0)
	{
		_StartTimeStamp = _FrameTimeStamp;
	}

	// ��� �����.
	GetRSI().Clear(_BackgroundColor);

	// ���� �ð�ȭ
	const InputManager& input = _GameEngine3.GetInputManager();

	if (input.IsReleased(InputButton::F1)) { _CurrentShowMode = ShowMode::Normal; }
	if (input.IsReleased(InputButton::F2)) { _CurrentShowMode = ShowMode::Wireframe; }
	if (input.IsReleased(InputButton::F3)) { _CurrentShowMode = ShowMode::DepthBuffer; }
}

void SoftRenderer::PostUpdate()
{
	// ������ ������.
	GetRSI().EndFrame();

	// �Է� ���� ������Ʈ
	GetGameEngine().GetInputManager().UpdateInput();

	// ���� ���� ������.
	_FrameCount++;
	INT64 currentTimeStamp = _PerformanceMeasureFunc();
	INT64 frameCycles = currentTimeStamp - _FrameTimeStamp;
	INT64 elapsedCycles = currentTimeStamp - _StartTimeStamp;
	_FrameTime = frameCycles / _CyclesPerMilliSeconds;
	_ElapsedTime = elapsedCycles / _CyclesPerMilliSeconds;
	_FrameFPS = _FrameTime == 0.f ? 0.f : 1000.f / _FrameTime;
	_AverageFPS = _ElapsedTime == 0.f ? 0.f : 1000.f / _ElapsedTime * _FrameCount;
}


