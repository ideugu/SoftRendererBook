
#include "Precompiled.h"
#include "Windows/WindowsGDI.h"

WindowsGDI::~WindowsGDI()
{
}

bool WindowsGDI::InitializeGDI(const ScreenPoint& InScreenSize)
{
	ReleaseGDI();

	_Handle = ::GetActiveWindow();
	if (_Handle == NULL)
	{
		return false;
	}

	if (_GDIInitialized)
	{
		DeleteObject(_DefaultBitmap);
		DeleteObject(DIBitmap);
		ReleaseDC(_Handle, _ScreenDC);
		ReleaseDC(_Handle, _MemoryDC);
	}

	_ScreenDC = GetDC(_Handle);
	if (_ScreenDC == NULL)
	{
		return false;
	}

	_MemoryDC = CreateCompatibleDC(_ScreenDC);
	if (_MemoryDC == NULL)
	{
		return false;
	}

	_ScreenSize = InScreenSize;

	// Color & Bitmap Setting
	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = _ScreenSize._X;
	bmi.bmiHeader.biHeight = -_ScreenSize._Y;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	DIBitmap = CreateDIBSection(_MemoryDC, &bmi, DIB_RGB_COLORS, (void**)&_ScreenBuffer, NULL, 0);
	if (DIBitmap == NULL)
	{
		return false;
	}

	_DefaultBitmap = (HBITMAP)SelectObject(_MemoryDC, DIBitmap);
	if (_DefaultBitmap == NULL)
	{
		return false;
	}

	// Create Depth Buffer
	CreateDepthBuffer();

	_GDIInitialized = true;
	return true;
}

void WindowsGDI::ReleaseGDI()
{
	if (_GDIInitialized)
	{
		DeleteObject(_DefaultBitmap);
		DeleteObject(DIBitmap);
		ReleaseDC(_Handle, _ScreenDC);
		ReleaseDC(_Handle, _MemoryDC);
	}

	if (_DepthBuffer != nullptr)
	{
		delete[] _DepthBuffer;
		_DepthBuffer = nullptr;
	}

	_GDIInitialized = false;
}


void WindowsGDI::FillBuffer(Color32 InColor)
{
	if (!_GDIInitialized || (_ScreenBuffer == NULL))
	{
		return;
	}

	Color32* dest = _ScreenBuffer;
	UINT32 totalCount = _ScreenSize._X * _ScreenSize._Y;
	CopyBuffer<Color32>(_ScreenBuffer, &InColor, totalCount);
	return;
}

template <class T>
T* WindowsGDI::CopyBuffer(T* InDst, T* InSrc, int InCount)
{
	if (InCount == 0)
	{
		return NULL;
	}

	if (InCount == 1)
	{
		memcpy(InDst, InSrc, sizeof(T));
	}
	else
	{
		int half = Math::FloorToInt(InCount * 0.5f);
		CopyBuffer<T>(InDst, InSrc, half);
		memcpy(InDst + half, InDst, half * sizeof(T));

		if (InCount % 2)
		{
			memcpy(InDst + (InCount - 1), InSrc, sizeof(T));
		}
	}

	return InDst;
}

Color32* WindowsGDI::GetScreenBuffer() const
{
	return _ScreenBuffer;
}

void WindowsGDI::DrawStatisticTexts()
{
	if (_StatisticTexts.size() == 0)
	{
		return;
	}

	HFONT hFont, hOldFont;
	hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
	if (hOldFont = (HFONT)SelectObject(_MemoryDC, hFont))
	{
		static const int leftPosition = 600;
		static const int topPosition = 10;
		static const int rowHeight = 20;
		int currentPosition = topPosition;
		for (std::vector<std::string>::const_iterator it = _StatisticTexts.begin(); it != _StatisticTexts.end(); ++it)
		{
			TextOut(_MemoryDC, leftPosition, currentPosition, (*it).c_str(), (int)((*it).length()));
			currentPosition += rowHeight;
		}

		SelectObject(_MemoryDC, hOldFont);
	}
}

void WindowsGDI::SwapBuffer()
{
	if (!_GDIInitialized)
	{
		return;
	}

	DrawStatisticTexts();
	BitBlt(_ScreenDC, 0, 0, _ScreenSize._X, _ScreenSize._Y, _MemoryDC, 0, 0, SRCCOPY);

	_StatisticTexts.clear();
}

void WindowsGDI::CreateDepthBuffer()
{
	_DepthBuffer = new float[_ScreenSize._X * _ScreenSize._Y];
}

void WindowsGDI::ClearDepthBuffer()
{
	if (_DepthBuffer != nullptr)
	{
		float* dest = _DepthBuffer;
		float defValue = INFINITY;
		UINT32 totalCount = _ScreenSize._X * _ScreenSize._Y;
		CopyBuffer<float>(_DepthBuffer, &defValue, totalCount);
	}
}

float WindowsGDI::GetDepthBufferValue(const ScreenPoint& InPos) const
{
	if (_DepthBuffer == nullptr)
	{
		return INFINITY;
	}

	if (!IsInScreen(InPos))
	{
		return INFINITY;
	}

	return *(_DepthBuffer + GetScreenBufferIndex(InPos));
}

void WindowsGDI::SetDepthBufferValue(const ScreenPoint& InPos, float InDepthValue)
{
	if (_DepthBuffer == nullptr)
	{
		return;
	}

	if (!IsInScreen(InPos))
	{
		return;
	}

	*(_DepthBuffer + GetScreenBufferIndex(InPos)) = InDepthValue;
}