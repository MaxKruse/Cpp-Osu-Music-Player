#pragma once
#include "Core.h"

namespace Parser
{
	// Timingpoints are basic timings that affect hitsounds played and the speed of the map, as well as volume.
	class API TimingPoint
	{
	public:
		TimingPoint(long offset, double milliseconds_per_beat, unsigned short sampleset, unsigned short sampleindex, unsigned short volume, bool inherited);

		inline const long GetOffset() const { return m_Offset; }
		inline const double GetMillisecondsPerBeat() const { return m_MillisecondsPerBeat; }
		inline const unsigned short GetSampleSet() const { return m_SampleSet; }
		inline const unsigned short GetSampleIndex() const { return m_SampleIndex; }
		inline const unsigned short GetVolume() const { return m_Volume; }
		inline const bool IsInherited() const { return m_Inherited; }

	private:
		long m_Offset;
		double m_MillisecondsPerBeat;
		unsigned short m_SampleSet;
		unsigned short m_SampleIndex;
		unsigned short m_Volume;
		bool m_Inherited;

	};
}  // namespace Parser
