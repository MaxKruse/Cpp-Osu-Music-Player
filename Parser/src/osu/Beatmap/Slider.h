#pragma once
#include "Hitobject.h"

namespace Parser {
	namespace Beatmap {

	// A Slider is the most complex Hitobject
	// lots of sounds possible due to repeats having their own sounds
	class API Slider : public Hitobject
	{
	public:
		Slider(unsigned short x, unsigned short y, long offset, unsigned short type, int hitsound, unsigned int repeat, std::vector<std::string> edgeHitsounds, std::vector<std::string> edgeAdditions, std::vector<std::string> extras, float durationWithoutBeatLength);
		~Slider()
		{
			LOGGER_DEBUG("\tSlider Destroyed");
		}
		std::vector<std::pair<long, std::vector<std::string>>> GetHitsounds(const TimingPoint& t) override;

	private:
		unsigned short m_Hitsound;
		unsigned int m_Repeat;
		float m_DurationWithoutBeatLength;
		std::vector<std::string> m_EdgeHitsounds;
		std::vector<std::string> m_EdgeAdditions;
		std::vector<std::string> m_Extras;



		// Taken from: https://stackoverflow.com/a/46931770
		std::vector<std::string> split(const std::string & s, const char & delim) {
			std::vector<std::string> result;
			std::stringstream ss(s);
			std::string item;

			while (std::getline(ss, item, delim)) {
				result.push_back(item);
			}

			return result;
		}
	};

	} // namespace Beatmap
} // namespace Parser
