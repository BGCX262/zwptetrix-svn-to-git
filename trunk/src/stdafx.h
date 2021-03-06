// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>


#include <memory>
#include <vector>

#include <Gdiplus.h>
#pragma comment(lib, "Gdiplus")
using namespace Gdiplus;

// TODO: reference additional headers your program requires here
#define ASSERT(e) assert(e)