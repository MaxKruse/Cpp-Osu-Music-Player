#include "pch.h"
#include "Logger.h"

#ifdef LINK_STATIC
	#define API_PLAYER
#elif defined(BUILD_DLL)
	#define API_PLAYER __declspec(dllexport)
#else
	#define API_PLAYER __declspec(dllimport)
#endif