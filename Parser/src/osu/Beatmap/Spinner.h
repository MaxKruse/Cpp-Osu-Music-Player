#pragma once
#include "Hitobject.h"

namespace Parser {
	namespace Beatmap {

	// A Spinner is the simplest Hitobject
	// One offset, Duration, a maximum of 4 possible hitsounds (including default)
	class API_PLAYER Spinner : public Hitobject
	{
	public:
		Spinner(unsigned short x, unsigned short y, long offset, unsigned short type, std::vector<std::shared_ptr<Hitsound>> hitsounds);
	};

	} // namespace Beatmap
} // namespace Parser