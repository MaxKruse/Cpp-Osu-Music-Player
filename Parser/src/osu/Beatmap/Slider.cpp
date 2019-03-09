#include "pch.h"
#include "Slider.h"

namespace Parser {
	namespace Beatmap {

		Slider::Slider(unsigned short x, unsigned short y, long offset, unsigned short type, std::vector<std::shared_ptr<Hitsound>> hitsounds)
			: Hitobject(x, y, offset, type, hitsounds)
		{
		}

	} // namespace Beatmap
} // namespace Parser