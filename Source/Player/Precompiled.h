#pragma once


#if (!defined(PLATFORM_WINDOWS) && !defined(WIN32))

	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <tchar.h>
	#include "WindowsUtil.h"

#endif


#include "MathHeaders.h"
#include <cassert>

#include "EngineHeaders.h"
#include "RenderingSoftwareInterface.h"

#include <string>

