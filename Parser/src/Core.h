#include "pch.h"
#include "Logger.h"


#ifdef BUILD_DLL
	#define API_PLAYER __declspec(dllexport)
#else
	#define API_PLAYER __declspec(dllimport)
#endif