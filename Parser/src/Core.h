#include "pch.h"
#include "Logger.h"

#ifdef BUILD_DLL
	#define API __declspec(dllexport)
#else
	#define API __declspec(dllimport)
#endif

namespace Parser {

	static size_t Random(size_t low = 0, size_t high = 0xFFFFFFF)
	{
	size_t index = (size_t)pow((rand() * 5) % high, rand() * (sqrt(2)));
	index = index % (high-low);
	index = index + low;
	return index;
	}
	
}

