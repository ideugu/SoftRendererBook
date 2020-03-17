
#pragma once

#include <memory>
#include "WindowsGDI.h"
#include "RenderingSoftwareInterface.h"

class WindowsGDI;
class WindowsRSI : public WindowsGDI, public RenderingSoftwareInterface
{
public:
	WindowsRSI() = default;
	~WindowsRSI();

public:
	virtual bool Init(const ScreenPoint& InScreenSize) override;
	virtual void Shutdown() override;
	virtual bool IsInitialized() const { return _GDIInitialized; }

	virtual void Clear(const LinearColor& InClearColor) override;
	virtual void BeginFrame() override;
	virtual void EndFrame() override;

	virtual void DrawPoint(const Vector2& InVectorPos, const LinearColor& InColor) override;

	virtual void DrawFullVerticalLine(int InX, const LinearColor& InColor) override;
	virtual void DrawFullHorizontalLine(int InY, const LinearColor& InColor) override;

	virtual void PushStatisticText(std::string InText) override;

private:
	FORCEINLINE void SetPixel(const ScreenPoint& InPos, const LinearColor& InColor);
};

FORCEINLINE void WindowsRSI::SetPixel(const ScreenPoint& InPos, const LinearColor& InColor)
{
	SetPixelOpaque(InPos, InColor);
}
