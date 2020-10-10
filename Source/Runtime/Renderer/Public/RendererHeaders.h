#pragma once

#include "2D/Vertex.h"
#include "3D/Vertex.h"
#include "RenderingSoftwareInterface.h"

#if defined(PLATFORM_WINDOWS)
#include <windows.h>
#include "Windows/WindowsGDI.h"
#include "Windows/WindowsRSI.h"
#endif

using namespace CK;
