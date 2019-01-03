#include "pch.h"
#include "Hitcircle.h"

namespace Parser {

	Hitcircle::Hitcircle(unsigned short x, unsigned short y, long offset, unsigned short type, int hitsound, std::vector<std::string> extras)
		: Hitobject(x,y,offset,type), m_Hitsound(hitsound), m_Extras(extras)
	{
	}

	Hitcircle::~Hitcircle()
	{
	}

	std::vector<std::string> Hitcircle::GetHitsounds(TimingPoint t)
	{
		auto s = std::vector<std::string>();
		s.reserve(4); // hitnormal + additions (clap, finish, whistle), there is no other possiblity

		auto sampleset = t.GetSampleSet();
		auto sampleindex = t.GetSampleIndex();

		auto set_extra = stoi(m_Extras[0]);
		auto addition_extra = stoi(m_Extras[1]);
		auto customindex_extra = stoi(m_Extras[2]);
		auto samplevloume_extra = stoi(m_Extras[3]);


		// Base Sound
		if (set_extra != 0)
		{
			if (set_extra == 1)
			{
				s.emplace_back("normal-hitnormal.wav");
			}
			else if (set_extra == 2)
			{
				s.emplace_back("soft-hitnormal.wav");
			}
			else if (set_extra == 3)
			{
				s.emplace_back("drum-hitnormal.wav");
			}

		}
		else
		{
			if (sampleset == 1)
			{
				s.emplace_back("normal-hitnormal.wav");
			}
			else if (sampleset == 2)
			{
				s.emplace_back("soft-hitnormal.wav");
			}
			else if (sampleset == 3)
			{
				s.emplace_back("drum-hitnormal.wav");
			}
			else
			{
				s.emplace_back("normal-hitnormal.wav");
			}
		}

		// Additions
		if (addition_extra != 0)
		{
			// Normal
			if (addition_extra == 1)
			{
				if (m_Hitsound & 1 << 1)
				{
					s.emplace_back("normal-hitwhistle.wav");
				}
				else if (m_Hitsound & 1 << 2)
				{
					s.emplace_back("normal-hitfinish.wav");
				}
				else if (m_Hitsound & 1 << 3)
				{
					s.emplace_back("normal-hitclap.wav");
				}
			}
			// Soft
			else if (addition_extra == 2)
			{
				if (m_Hitsound & 1 << 1)
				{
					s.emplace_back("soft-hitwhistle.wav");
				}
				else if (m_Hitsound & 1 << 2)
				{
					s.emplace_back("soft-hitfinish.wav");
				}
				else if (m_Hitsound & 1 << 3)
				{
					s.emplace_back("soft-hitclap.wav");
				}
			}
			// Drum
			else if (addition_extra == 3)
			{
				if (m_Hitsound & 1 << 1)
				{
					s.emplace_back("drum-hitwhistle.wav");
				}
				else if (m_Hitsound & 1 << 2)
				{
					s.emplace_back("drum-hitfinish.wav");
				}
				else if (m_Hitsound & 1 << 3)
				{
					s.emplace_back("drum-hitclap.wav");
				}
			}

		}
		
		return s;
	}

}