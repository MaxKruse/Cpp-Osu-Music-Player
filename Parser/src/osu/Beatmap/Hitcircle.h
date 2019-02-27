#pragma once
#include "Hitobject.h"

namespace Parser {
	namespace Beatmap {

		// A Hitcircle is the 2nd simplest Hitobject, next to a spinner
		// One offset, a maximum of 4 possible hitsounds (including default)
		class API Hitcircle : public Hitobject
		{
		public:
			Hitcircle(unsigned short x, unsigned short y, long offset, unsigned short type, std::vector<Hitsound> hitsounds);

		private:
			std::vector<std::string> m_Extras;
		};
	} // namespace Beatmap
}  // namespace Parser
