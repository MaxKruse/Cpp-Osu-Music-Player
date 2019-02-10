#pragma once
#include "Hitobject.h"

namespace Parser {

	// A Spinner is the simplest Hitobject
	// One offset, Duration, a maximum of 4 possible hitsounds (including default)
	class API Spinner : public Hitobject
	{
	public:
		Spinner(unsigned short x, unsigned short y, long offset, unsigned short type, int hitsound, long end_offset, std::vector<std::string> extras);
		// Example Line: 256,192,87518,12,0,89299,0:0:0:0:
		std::pair<long, std::vector<std::string>> GetHitsounds(TimingPoint t) override;

	private:
		long m_EndOffset;
		unsigned short m_Hitsound;
		std::vector<std::string> m_Extras;
	};

}  // namespace Parser