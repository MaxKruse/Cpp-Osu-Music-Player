#include "pch.h"
#include "Hitcircle.h"

namespace Parser {
	namespace Beatmap {

		Hitcircle::Hitcircle(unsigned short x, unsigned short y, long offset, unsigned short type, std::string myLine)
			: Hitobject(x, y, offset, type, myLine)
		{
		}
	} // namespace Beatmap
} // namespace Parser