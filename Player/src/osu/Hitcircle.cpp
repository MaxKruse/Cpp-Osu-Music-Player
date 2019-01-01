#include "pch.h"
#include "Hitcircle.h"

namespace Parser {

	Hitcircle::Hitcircle(unsigned short x, unsigned short y, long offset, unsigned short type, int hitsound, std::vector<std::string> extras)
		: Hitobject(x,y,offset,type), m_Hitsound(hitsound), m_Extras(extras)
	{
	}

	std::vector<std::string> Hitcircle::GetHitsounds(TimingPoint t)
	{
		auto s = std::vector<std::string>();
		s.reserve(4); // hitnormal + additions (clap, finish, whistle), there is no other possiblity

		// TODO: Put strings for filenames in s
		auto sampleset = t.GetSampleSet();
		auto sampleindex = t.GetSampleIndex();

		bool special_sound = false;
		for (auto& sound : m_Extras)
		{
			if (stoi(sound) != 0)
				special_sound = true;
		}

		if (!special_sound)
		{
			switch (sampleset)
			{

			case 1:
			{
				s.push_back("normal-hitnormal.wav");
				break;
			}
			case 2:
			{
				s.push_back("soft-hitnormal.wav");
				break;
			}
			case 3:
			{
				s.push_back("drum-hitnormal.wav");
				break;
			}

			default:
			{
				s.push_back("normal-hitnormal.wav");
				break;
			}

			}
		}
		
		return s;
	}

}