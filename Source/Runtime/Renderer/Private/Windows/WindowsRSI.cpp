
#include "Precompiled.h"
#include "Windows/WindowsRSI.h"
#include "Windows/WindowsGDI.h"

WindowsRSI::~WindowsRSI()
{
}

bool WindowsRSI::Init(const ScreenPoint& InScreenSize)
{
	return InitializeGDI(InScreenSize);
}

void WindowsRSI::Shutdown()
{
	ReleaseGDI();
}

void WindowsRSI::Clear(const LinearColor & InClearColor)
{
	FillBuffer(InClearColor.ToColor32());
	ClearDepthBuffer();
}

void WindowsRSI::BeginFrame()
{
}

void WindowsRSI::EndFrame()
{
	SwapBuffer();
}

void WindowsRSI::DrawFullVerticalLine(int InX, const LinearColor & InColor)
{
	if (InX < 0 || InX >= _ScreenSize._X)
	{
		return;
	}

	for (int y = 0; y < _ScreenSize._Y; ++y)
	{
		SetPixel(ScreenPoint(InX, y), InColor);
	}
}

void WindowsRSI::DrawFullHorizontalLine(int InY, const LinearColor & InColor)
{
	if (InY < 0 || InY >= _ScreenSize._Y)
	{
		return;
	}

	for (int x = 0; x < _ScreenSize._X; ++x)
	{
		SetPixel(ScreenPoint(x, InY), InColor);
	}
}

void WindowsRSI::DrawPoint(const Vector2& InVectorPos, const LinearColor& InColor)
{
	SetPixel(ScreenPoint::ToScreenCoordinate(_ScreenSize, InVectorPos), InColor);
}

void WindowsRSI::DrawLine(const Vector2& InStartPos, const Vector2& InEndPos, const LinearColor& InColor)
{
	ScreenPoint startPosition = ScreenPoint::ToScreenCoordinate(_ScreenSize, InStartPos);
	ScreenPoint endPosition = ScreenPoint::ToScreenCoordinate(_ScreenSize, InEndPos);

	int width = endPosition._X - startPosition._X;
	int height = endPosition._Y - startPosition._Y;

	bool isGradualSlope = (Math::Abs(width) >= Math::Abs(height));
	int dx = (width >= 0) ? 1 : -1;
	int dy = (height > 0) ? 1 : -1;
	int fw = dx * width;
	int fh = dy * height;

	int f = isGradualSlope ? fh * 2 - fw : 2 * fw - fh;
	int f1 = isGradualSlope ? 2 * fh : 2 * fw;
	int f2 = isGradualSlope ? 2 * (fh - fw) : 2 * (fw - fh);
	int x = startPosition._X;
	int y = startPosition._Y;

	if (isGradualSlope)
	{
		while (x != endPosition._X)
		{
			SetPixel(ScreenPoint(x, y), InColor);

			if (f < 0)
			{
				f += f1;
			}
			else
			{
				f += f2;
				y += dy;
			}

			x += dx;
		}
	}
	else
	{
		while (y != endPosition._Y)
		{
			SetPixel(ScreenPoint(x, y), InColor);

			if (f < 0)
			{
				f += f1;
			}
			else
			{
				f += f2;
				x += dx;
			}

			y += dy;
		}
	}
}

void WindowsRSI::PushStatisticText(std::string InText)
{
	_StatisticTexts.emplace_back(InText);
}
