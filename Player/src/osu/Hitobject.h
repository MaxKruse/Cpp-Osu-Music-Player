#pragma once
#include "pch.h"
#include "Logger.h"
#include "TimingPoint.h"

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
		Hitobject(unsigned short x, unsigned short y, long offset, unsigned short type)
			: m_Position(x, y), m_Offset(offset), m_Type(type)
		{
		}	// TODO(Max): x, y, time, type, >> hitSound..., extras <<
		~Hitobject()
		= default;

		virtual std::vector<std::string> GetHitsounds(TimingPoint t) { return std::vector<std::string>(); }
		
		inline const unsigned short GetX() const { return m_Position.X; }
		inline const unsigned short GetY() const { return m_Position.Y; }
		inline const long GetOffset() const { return m_Offset; }
		inline const unsigned short GetType() const { return m_Type; }
			   
	protected:
		Position       m_Position;
		long           m_Offset;
		unsigned short m_Type;
	};

}  // namespace Parser