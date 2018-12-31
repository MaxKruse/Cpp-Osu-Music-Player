#include "pch.h"
#include "Hitobject.h"

namespace Parser {

	Hitobject::Hitobject(unsigned short x, unsigned short y, long offset, unsigned short type)
		: m_Position(x,y), m_Offset(offset), m_Type(type)
	{
	}

	Hitobject::~Hitobject()
	{
	}

}