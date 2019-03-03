#include "pch.h"
#include "Spinner.h"

namespace Parser {
	namespace Beatmap {

	Spinner::Spinner(unsigned short x, unsigned short y, long offset, unsigned short type, std::vector<Hitsound*> hitsounds)
		: Hitobject(x, y, offset, type, hitsounds)
	{
	}

	} // namespace Beatmap
} // namespace Parser