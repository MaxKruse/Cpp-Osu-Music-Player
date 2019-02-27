#pragma once
#include "Core.h"

namespace Parser {
	namespace Beatmap {
		// Timingpoints are basic timings that affect hitsounds played and the speed of the map, as well as volume.
		class API TimingPoint
		{
		public:
			TimingPoint()
				: m_Offset(-1), m_MillisecondsPerBeat(-1.0), m_SampleIndex(5), m_SampleSet(5), m_Volume(150)
			{}
			TimingPoint(long offset, float milliseconds_per_beat, unsigned short sampleset, unsigned short sampleindex, unsigned short volume, bool inherited);

			inline const long GetOffset() const { return m_Offset; }
			inline const double GetMillisecondsPerBeat() const { return m_MillisecondsPerBeat; }
			const std::string GetSampleSet() const
			{
				if (m_SampleSet == 2)
				{
					return "soft";
				}

				if (m_SampleSet == 3)
				{
					return "drum";
				}

				return "normal";
			}

			const std::string GetSampleIndex() const
			{
				return std::to_string(m_SampleIndex);
			}

			inline const unsigned short GetVolume() const { return m_Volume; }
			inline const bool IsInherited() const { return m_Inherited; }
			inline const double GetBPM() const { return 60000.0 / GetMillisecondsPerBeat(); }
			inline const double GetSvMultiplier() const { return -100.0 / GetMillisecondsPerBeat(); }

			std::string ToString() const;

			inline const bool HasDefaults() const { return (m_Offset == -1 || m_MillisecondsPerBeat == -1 || m_SampleIndex == 5 || m_SampleSet == 5 || m_Volume == 150); }

		private:
			long m_Offset;
			double m_MillisecondsPerBeat;
			unsigned short m_SampleSet;
			unsigned short m_SampleIndex;
			unsigned short m_Volume;
			bool m_Inherited;

		};

		inline std::ostream& operator<<(std::ostream& os, const TimingPoint& t)
		{
			return os << t.ToString();
		}
	} // namespace Beatmap
} // namespace Parser