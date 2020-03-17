#pragma once

#include <Windows.h>

namespace WindowsUtil
{
	void GetWindowSize(HWND handle, float* width, float* height)
	{
		RECT rect;
		::GetClientRect(handle, &rect);
		*width = static_cast<float>(rect.right - rect.left);
		*height = static_cast<float>(rect.bottom - rect.top);
	}

	void Show(HWND handle)
	{
		::ShowWindow(handle, SW_SHOW);
		::SetForegroundWindow(handle);
		::SetFocus(handle);
	}

	void CenterWindow(HWND handle)
	{
		// center on parent or screen
		RECT rc, rcOwner, rcWindow;
		HWND ownerHandle = GetDesktopWindow();
		GetWindowRect(ownerHandle, &rcOwner);
		GetWindowRect(handle, &rcWindow);
		CopyRect(&rc, &rcOwner);

		OffsetRect(&rcWindow, -rcWindow.left, -rcWindow.top);
		OffsetRect(&rc, -rc.left, -rc.top);
		OffsetRect(&rc, -rcWindow.right, -rcWindow.bottom);

		::SetWindowPos(handle, HWND_TOP,
			rcOwner.left + (rc.right / 2),
			rcOwner.top + (rc.bottom / 2),
			0, 0, 
			SWP_NOSIZE);
	}

	float GetCyclesPerMilliSeconds()
	{
		LARGE_INTEGER frequency;
		if (!QueryPerformanceFrequency(&frequency))
		{
			return 0.f;
		}

		return (float)(frequency.QuadPart / 1000.f);
	}

	long long GetCurrentTimeStamp()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		return currentTime.QuadPart;
	}

	float GetXAxisInput()
	{
		bool isLeft = GetAsyncKeyState(VK_LEFT);
		bool isRight = GetAsyncKeyState(VK_RIGHT);
		if (isLeft ^ isRight)
		{
			return isLeft ? -1.f : 1.f;
		}
		return 0.f;
	}

	float GetYAxisInput()
	{
		bool isDown = GetAsyncKeyState(VK_DOWN);
		bool isUp = GetAsyncKeyState(VK_UP);
		if (isDown ^ isUp)
		{
			return isDown ? -1.f : 1.f;
		}
		return 0.f;
	}

	#define ISPRESSED(KeyCode) (::GetKeyState(KeyCode) & 0x8000) != 0

	bool SpacePressedInput()
	{
		return ISPRESSED(VK_SPACE);
	}

	void BindInput(InputManager& InInputManager)
	{
		InInputManager.GetXAxis = WindowsUtil::GetXAxisInput;
		InInputManager.GetYAxis = WindowsUtil::GetYAxisInput;
		InInputManager.SpacePressed = WindowsUtil::SpacePressedInput;
	}
}