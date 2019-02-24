#pragma once
#include "Hitobject.h"

namespace Parser {
	namespace Beatmap {

	// A Spinner is the simplest Hitobject
	// One offset, Duration, a maximum of 4 possible hitsounds (including default)
	class API Spinner : public Hitobject
	{
	public:
		Spinner(unsigned short x, unsigned short y, long offset, unsigned short type, int hitsound, long end_offset, std::vector<std::string> extras);
		~Spinner()
		{
			LOGGER_DEBUG("\tSpinner Destroyed");
		}
		// Example Line: 256,192,87518,12,0,89299,0:0:0:0:
		std::vector<std::pair<long, std::vector<std::string>>> GetHitsounds(const TimingPoint& t) override;

	private:
		long m_EndOffset;
		unsigned short m_Hitsound;
		std::vector<std::string> m_Extras;
	};

	} // namespace Beatmap
} // namespace Parser