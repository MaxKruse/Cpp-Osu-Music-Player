#include "pch.h"
#include "Spinner.h"

namespace Parser {
	namespace Beatmap {

	Spinner::Spinner(unsigned short x, unsigned short y, long offset, unsigned short type, std::string myLine)
		: Hitobject(x, y, offset, type, myLine)
	{
	}

	} // namespace Beatmap
} // namespace Parser