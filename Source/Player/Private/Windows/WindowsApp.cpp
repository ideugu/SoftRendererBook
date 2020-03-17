
#include "Precompiled.h"
#include "Windows/WindowsRSI.h"
#include "WindowsPlayer.h"
#include "WindowsUtil.h"
#include "SoftRenderer.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	ScreenPoint defScreenSize(800, 600);
	SoftRenderer instance(new WindowsRSI());
	WindowsPlayer::gOnResizeFunc = [&instance](const ScreenPoint& InNewScreenSize) { 
		if (InNewScreenSize.HasZero()) {
			return;
		}
		instance.OnResize(InNewScreenSize); 
	};
	instance.PerformanceInitFunc = WindowsUtil::GetCyclesPerMilliSeconds;
	instance.PerformanceMeasureFunc = WindowsUtil::GetCurrentTimeStamp;
	WindowsUtil::BindInput(instance.GetGameEngine().GetInputManager());

	if (!WindowsPlayer::Create(hInstance, defScreenSize))
	{
		return -1;
	}

	WindowsUtil::Show(WindowsPlayer::gHandle);
	WindowsUtil::CenterWindow(WindowsPlayer::gHandle);

	static float previousTimer = 0.f;
	static float updatePeriod = 500.f;
	while (WindowsPlayer::Tick())
	{
		instance.OnTick();

		float currentTime = instance.GetElapsedTime();
		if (currentTime - previousTimer > updatePeriod)
		{
			float frameFPS = instance.GetFrameFPS();
			WindowsPlayer::SetWindowsStatTitle(frameFPS);
			previousTimer = currentTime;
		}
	}

	instance.OnShutdown();
	WindowsPlayer::Destroy();
	return 0;
}
