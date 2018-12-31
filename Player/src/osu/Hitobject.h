#pragma once
#include "pch.h"
#include "Logger.h"

namespace Parser {

	struct Position
	{
		unsigned short X;
		unsigned short Y;

		Position(unsigned short x, unsigned short y)
			: X(x), Y(y)
		{
		}
	};

	class Hitobject
	{
	public:
		Hitobject(unsigned short x, unsigned short y, long offset, unsigned short type); // TODO: x, y, time, type, >> hitSound..., extras <<
		~Hitobject();

		inline const unsigned short GetX() const { return m_Position.X; }
		inline const unsigned short GetY() const { return m_Position.Y; }
		inline const long GetOffset() const { return m_Offset; }
		inline const unsigned short GetType() const { return m_Type; }

		// TODO: return a vector of vectors for all hitsounds for this hitobject, format example:
		// std::map<long offset, std::vector<std::string> hitsounds>


	protected:
		Position       m_Position;
		long           m_Offset;
		unsigned short m_Type;
	};

}