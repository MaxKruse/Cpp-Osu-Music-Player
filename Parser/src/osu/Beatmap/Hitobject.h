#pragma once
#include "Core.h"
#include "TimingPoint.h"
#include "Hitsound.h"

namespace Parser {
	namespace Beatmap {

		enum HITOBJECT_TYPE
		{
			HITCIRCLE = 1 << 0,
			SLIDER = 1 << 1,
			SPINNER = 1 << 3
		};

		// Basic struct to safe memory
		struct API_PLAYER Position
		{
			unsigned short X;
			unsigned short Y;

			Position(unsigned short x, unsigned short y)
				: X(x), Y(y)
			{
			}
		};

		class API_PLAYER Hitobject
		{
		public:
			Hitobject(unsigned short x, unsigned short y, long offset, unsigned short type, std::string myLine)
				: m_Position(x, y), m_Type(type), m_Line(myLine)
			{
				// Hitobjects always have a starting offset, BUT different types might have other Offsets
				m_Offset.emplace_back(offset);
			}

			inline const unsigned short						GetX() const { return m_Position.X; }
			inline const unsigned short						GetY() const { return m_Position.Y; }
			inline const std::vector<long>					GetOffsets() const { return m_Offset; }
			const std::vector<std::shared_ptr<Hitsound>>	GetHitsounds() const { return m_Hitsounds; }
			virtual unsigned short							GetType() = 0;
			virtual void AddHitsounds(std::vector<std::shared_ptr<Hitsound>> a) { m_Hitsounds = a; }
			inline const std::string						GetLine() const { return m_Line; }

		protected:
			
			Position									m_Position;
			std::vector<long>							m_Offset;
			unsigned short								m_Type;
			std::vector<std::shared_ptr<Hitsound>>		m_Hitsounds;
			std::string m_Line;
		};

	} // namespace Beatmap
}  // namespace Parser