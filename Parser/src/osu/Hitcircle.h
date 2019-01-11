#pragma once
#include "Hitobject.h"

namespace Parser {

	class API Hitcircle : public Hitobject
	{
	public:
		Hitcircle(unsigned short x, unsigned short y, long offset, unsigned short type, int hitsound, std::vector<std::string> extras);

		std::vector<std::string> GetHitsounds(TimingPoint t) override;

	private:
		unsigned short m_Hitsound;
		std::vector<std::string> m_Extras;
	};

}  // namespace Parser