#pragma once
#include "Hitobject.h"

namespace Parser {
	namespace Beatmap {

		// A Hitcircle is the 2nd simplest Hitobject, next to a spinner
		// One offset, a maximum of 4 possible hitsounds (including default)
		class API_PLAYER Hitcircle : public Hitobject
		{
		public:
			Hitcircle(unsigned short x, unsigned short y, long offset, unsigned short type, std::string myLine);
			
			unsigned short GetType() override
			{
				return HITOBJECT_TYPE::HITCIRCLE;
			}

		private:
			std::vector<std::string> m_Extras;
		};
	} // namespace Beatmap
}  // namespace Parser
