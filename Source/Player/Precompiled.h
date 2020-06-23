#pragma once


#if (!defined(PLATFORM_WINDOWS) && !defined(WIN32))

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include "WindowsUtil.h"

#endif


#include "MathHeaders.h"
#include "EngineHeaders.h"
#include "RendererHeaders.h"

using namespace CK;
