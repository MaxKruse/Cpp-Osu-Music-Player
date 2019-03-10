#pragma once
#include "Hitobject.h"

namespace Parser {
	namespace Beatmap {

	// A Slider is the most complex Hitobject
	// lots of sounds possible due to repeats having their own sounds
		class API_PLAYER Slider : public Hitobject
		{
		public:
			Slider(unsigned short x, unsigned short y, long offset, unsigned short type, std::vector<std::shared_ptr<Hitsound>> hitsounds);

		};
	} // namespace Beatmap
} // namespace Parser
