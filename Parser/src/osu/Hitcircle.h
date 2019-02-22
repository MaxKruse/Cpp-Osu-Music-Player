#pragma once
#include "Hitobject.h"

namespace Parser {

	// A Hitcircle is the 2nd simplest Hitobject, next to a spinner
	// One offset, a maximum of 4 possible hitsounds (including default)
	class API Hitcircle : public Hitobject
	{
	public:
		Hitcircle(unsigned short x, unsigned short y, long offset, unsigned short type, int hitsound, std::vector<std::string> extras);
		~Hitcircle()
		{
			LOGGER_DEBUG("\tHitcircle Destroyed");
		}
		std::vector<std::pair<long, std::vector<std::string>>> GetHitsounds(const TimingPoint& t) override;

	private:
		unsigned short m_Hitsound;
		std::vector<std::string> m_Extras;
	};

}  // namespace Parser
