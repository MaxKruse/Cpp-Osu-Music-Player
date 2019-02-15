#include "pch.h"
#include "Slider.h"

namespace Parser {

	Slider::Slider(unsigned short x, unsigned short y, long offset, unsigned short type, int hitsound, unsigned int repeat, std::vector<std::string> edgeHitsounds, std::vector<std::string> edgeAdditions, std::vector<std::string> extras, float durationWithoutBeatLength)
		: Hitobject(x,y,offset,type), m_Repeat(repeat), m_EdgeHitsounds(edgeHitsounds), m_EdgeAdditions(edgeAdditions), m_Extras(extras), m_DurationWithoutBeatLength(durationWithoutBeatLength)
	{
	}

	// For the timingpoint that affects this Hitcircle, give a pair of:
	// [Offset] = vector("sound1", "sound2")
	std::vector<std::pair<long, std::vector<std::string>>> Slider::GetHitsounds(const TimingPoint& t)
	{
		m_DurationWithoutBeatLength *= t.GetMillisecondsPerBeat();

		auto s = std::vector<std::string>();
		auto sampleset = t.GetSampleSet();
		auto sampleindex = t.GetSampleIndex();

		auto set_extra = stoi(m_Extras.at(0));
		auto addition_extra = stoi(m_Extras.at(1));
		auto customindex_extra = stoi(m_Extras.at(2));
		auto samplevolume_extra = stoi(m_Extras.at(3));

		// This is a mess, will have to redo later to comment on this

		// Custom Hitsounds
		std::string customIndex = "";

		if (sampleindex > 2)
		{
			customIndex = std::to_string(sampleindex);
		}

		customIndex = customindex_extra > 1 ? std::to_string(customindex_extra) : "";

		std::string sampleSetString = "normal-";

		if (sampleset == 1)
		{
			sampleSetString = "normal-";
		}
		else if (sampleset == 2)
		{
			sampleSetString = "soft-";
		}
		else if (sampleset == 3)
		{
			sampleSetString = "drum-";
		}

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
			s.emplace_back(sampleSetString + "hitnormal.wav");
		}

		// Additions
		if (m_Hitsound & (1 << 1))
		{
			s.emplace_back(sampleSetString + "hitwhistle" + customIndex + ".wav");
		}
		if (m_Hitsound & (1 << 2))
		{
			s.emplace_back(sampleSetString + "hitfinish" + customIndex + ".wav");
		}
		if (m_Hitsound & (1 << 3))
		{
			s.emplace_back(sampleSetString + "hitclap" + customIndex + ".wav");
		}

		std::vector<std::pair<long, std::vector<std::string>>> r;
		std::pair<long, std::vector<std::string>> m(m_Offset.at(0), s);
		r.emplace_back(m);
		return r;
	}

}
