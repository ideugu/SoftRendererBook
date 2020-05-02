
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
	if (InX < 0 || InX >= _ScreenSize.X)
	{
		return;
	}

	for (int y = 0; y < _ScreenSize.Y; ++y)
	{
		SetPixel(ScreenPoint(InX, y), InColor);
	}
}

void WindowsRSI::DrawFullHorizontalLine(int InY, const LinearColor & InColor)
{
	if (InY < 0 || InY >= _ScreenSize.Y)
	{
		return;
	}

	for (int x = 0; x < _ScreenSize.X; ++x)
	{
		SetPixel(ScreenPoint(x, InY), InColor);
	}
}

void WindowsRSI::DrawPoint(const Vector2& InVectorPos, const LinearColor& InColor)
{
	SetPixel(ScreenPoint::ToScreenCoordinate(_ScreenSize, InVectorPos), InColor);
}

int WindowsRSI::TestRegion(const Vector2& InVectorPos, const Vector2& InMinPos, const Vector2& InMaxPos)
{
	int result = 0;
	if (InVectorPos.X < InMinPos.X)
	{
		result = 1;
	}
	else if (InVectorPos.X > InMaxPos.X)
	{
		result = (1 << 1);
	}

	if (InVectorPos.Y < InMinPos.Y)
	{
		result = (1 << 2);
	}
	else if (InVectorPos.Y > InMaxPos.Y)
	{
		result = (1 << 3);
	}

	return result;
}

bool WindowsRSI::CohenSutherlandLineClip(Vector2& InOutStartPos, Vector2& InOutEndPos, const Vector2& InMinPos, const Vector2& InMaxPos)
{
	int startTest = TestRegion(InOutStartPos, InMinPos, InMaxPos);
	int endTest = TestRegion(InOutEndPos, InMinPos, InMaxPos);

	float width = (InOutEndPos.X - InOutStartPos.X);
	float height = (InOutEndPos.Y - InOutStartPos.Y);

	while (true)
	{
		if ((startTest == 0) && (endTest == 0))
		{
			return true;
		}
		else if (startTest & endTest)
		{
			return false;
		}
		else
		{
			Vector2 clippedPosition;
			int currentTest;
			if (startTest != 0)
			{
				currentTest = startTest;
			}
			else
			{
				currentTest = endTest;
			}

			if (currentTest < (1 << 2))
			{
				if (currentTest & 1)
				{
					clippedPosition.X = InMinPos.X;
				}
				else
				{
					clippedPosition.X = InMaxPos.X;
				}

				if (height == 0)
				{
					clippedPosition.Y = InOutStartPos.Y;

				}
				else
				{
					clippedPosition.Y = InOutStartPos.Y + height * (clippedPosition.X - InOutStartPos.X) / width;
				}
			}
			else
			{
				if (currentTest & (1 << 2))
				{
					clippedPosition.Y = InMinPos.Y;
				}
				else
				{
					clippedPosition.Y = InMaxPos.Y;
				}

				if (width == 0)
				{
					clippedPosition.X = InOutStartPos.X;

				}
				else
				{
					clippedPosition.X = InOutStartPos.X + width * (clippedPosition.Y - InOutStartPos.Y) / height;
				}
			}

			if (currentTest == startTest)
			{
				InOutStartPos = clippedPosition;
				startTest = TestRegion(InOutStartPos, InMinPos, InMaxPos);
			}
			else
			{
				InOutEndPos = clippedPosition;
				endTest = TestRegion(InOutEndPos, InMinPos, InMaxPos);
			}
		}
	}

	return true;
}

void WindowsRSI::DrawLine(const Vector2& InStartPos, const Vector2& InEndPos, const LinearColor& InColor)
{
	Vector2 clippedStart = InStartPos;
	Vector2 clippedEnd = InEndPos;
	Vector2 screenExtend = Vector2(_ScreenSize.X, _ScreenSize.Y) * 0.5f;
	Vector2 minScreen = -screenExtend;
	Vector2 maxScreen = screenExtend;
	if (!CohenSutherlandLineClip(clippedStart, clippedEnd, minScreen, maxScreen))
	{
		return;
	}

	ScreenPoint startPosition = ScreenPoint::ToScreenCoordinate(_ScreenSize, clippedStart);
	ScreenPoint endPosition = ScreenPoint::ToScreenCoordinate(_ScreenSize, clippedEnd);

	int width = endPosition.X - startPosition.X;
	int height = endPosition.Y - startPosition.Y;

	bool isGradualSlope = (Math::Abs(width) >= Math::Abs(height));
	int dx = (width >= 0) ? 1 : -1;
	int dy = (height > 0) ? 1 : -1;
	int fw = dx * width;
	int fh = dy * height;

	int f = isGradualSlope ? fh * 2 - fw : 2 * fw - fh;
	int f1 = isGradualSlope ? 2 * fh : 2 * fw;
	int f2 = isGradualSlope ? 2 * (fh - fw) : 2 * (fw - fh);
	int x = startPosition.X;
	int y = startPosition.Y;

	if (isGradualSlope)
	{
		while (x != endPosition.X)
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
		while (y != endPosition.Y)
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

void WindowsRSI::PushStatisticText(std::string && InText)
{
	_StatisticTexts.emplace_back(InText);
}

void WindowsRSI::PushStatisticTexts(std::vector<std::string> && InTexts)
{
	std::move(InTexts.begin(), InTexts.end(), std::back_inserter(_StatisticTexts));
}
