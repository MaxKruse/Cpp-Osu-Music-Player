#include "pch.h"
#include "Slider.h"

namespace Parser {
	namespace Beatmap {

		Slider::Slider(unsigned short x, unsigned short y, long offset, unsigned short type, std::string myLine)
			: Hitobject(x, y, offset, type, myLine)
		{
		}

	} // namespace Beatmap
} // namespace Parser