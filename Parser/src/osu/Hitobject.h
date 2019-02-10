#pragma once
#include "Core.h"
#include "Logger.h"
#include "TimingPoint.h"

namespace Parser {

	enum HITOBJECT_TYPE
	{
		HITCIRCLE	= 1 << 0,
		SLIDER		= 1 << 1,
		SPINNER		= 1 << 3
	};

	// Basic struct to safe memory
	struct API Position
	{
		unsigned short X;
		unsigned short Y;

		Position(unsigned short x, unsigned short y)
			: X(x), Y(y)
		{
		}
	};

	class API Hitobject
	{
	public:
		Hitobject(unsigned short x, unsigned short y, long offset, unsigned short type)
			: m_Position(x, y), m_Type(type)
		{
			// Hitobjects always have a starting offset, BUT different types might have other Offsets
			m_Offset.emplace_back(offset);
		}

		virtual ~Hitobject() = default;

		virtual std::pair<long, std::vector<std::string>> GetHitsounds(TimingPoint t) = 0;
		
		inline const unsigned short    GetX() const { return m_Position.X; }
		inline const unsigned short    GetY() const { return m_Position.Y; }
		inline const std::vector<long> GetOffsets() const { return m_Offset; }
		inline const unsigned short    GetType() const { return m_Type; }
			   
	protected:
		Position                    m_Position;
		std::vector<long>           m_Offset;
		unsigned short              m_Type;
	};

}  // namespace Parser