#include "pch.h"
#include "Slider.h"

namespace Parser {
	namespace Beatmap {

		Slider::Slider(unsigned short x, unsigned short y, long offset, unsigned short type, int hitsound, unsigned int repeat, std::vector<std::string> edgeHitsounds, std::vector<std::string> edgeAdditions, std::vector<std::string> extras, float durationWithoutBeatLength)
			: Hitobject(x, y, offset, type), m_Repeat(repeat), m_EdgeHitsounds(edgeHitsounds), m_EdgeAdditions(edgeAdditions), m_Extras(extras), m_DurationWithoutBeatLength(durationWithoutBeatLength)
		{
		}

		// For the timingpoint that affects this Slider, give a vector pair of:
		// [Offset] = vector("sound1", "sound2")
		std::vector<std::pair<long, std::vector<std::string>>> Slider::GetHitsounds(const TimingPoint& t)
		{
			m_DurationWithoutBeatLength *= t.GetMillisecondsPerBeat();
			std::vector<std::pair<long, std::vector<std::string>>> r;

			// Set Offsets
			if (m_Offset.size() == 1)
			{
				for (size_t i = 0; i < m_Repeat; i++)
				{
					m_Offset.emplace_back(m_DurationWithoutBeatLength * (i + 1) + m_Offset.at(0));
				}
			}

			for (size_t i = 0; i < m_Repeat; i++)
			{
				auto s = std::vector<std::string>();
				auto sampleset = t.GetSampleSet();
				auto sampleindex = t.GetSampleIndex();

				// Custom Hitsounds
				std::string customIndex = "";

				if (sampleindex > 2)
				{
					customIndex = std::to_string(sampleindex);
				}

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

				bool fail = false;
				if (m_EdgeAdditions.size() - 1 < i || m_EdgeHitsounds.size() - 1 < i)
				{
					s.emplace_back(sampleSetString + "hitnormal.wav");
					LOGGER_DEBUG("Slider hitsounds not formated correctly. Giving default");
					fail = true;
				}

				if (!fail)
				{
					auto Hitsound = stoi(m_EdgeHitsounds.at(i));
					auto Addition = split(m_EdgeAdditions.at(i), ':');

					auto set_extra = stoi(Addition.at(0));
					auto addition_extra = stoi(Addition.at(1));

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
					if (Hitsound & (1 << 1))
					{
						s.emplace_back(sampleSetString + "hitwhistle" + customIndex + ".wav");
					}
					if (Hitsound & (1 << 2))
					{
						s.emplace_back(sampleSetString + "hitfinish" + customIndex + ".wav");
					}
					if (Hitsound & (1 << 3))
					{
						s.emplace_back(sampleSetString + "hitclap" + customIndex + ".wav");
					}

				}
				std::pair<long, std::vector<std::string>> m(m_Offset.at(i), s);
				r.emplace_back(m);
			}
			return r;
		}

	} // namespace Beatmap
} // namespace Parser