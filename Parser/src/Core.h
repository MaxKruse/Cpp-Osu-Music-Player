#include "pch.h"

#include "cxxtimer.hpp"
#include "Logger.h"

#ifdef BUILD_DLL
	#define API __declspec(dllexport)
#else
	#define API __declspec(dllimport)
#endif