#pragma once
#include "pch.h"
#include "Hitobject.h"
#include "TimingPoint.h"

namespace Parser {

	class Hitcircle : public Hitobject
	{
	public:
		Hitcircle(unsigned short x, unsigned short y, long offset, unsigned short type);

	};

}