#include "pch.h"
#include "TimingPoint.h"

namespace Parser {
	namespace Beatmap {

	TimingPoint::TimingPoint(long offset, double milliseconds_per_beat, unsigned short sampleset, unsigned short sampleindex, unsigned short volume, bool inherited)
		: m_Offset(offset), m_MillisecondsPerBeat(milliseconds_per_beat), m_SampleSet(sampleset), m_SampleIndex(sampleindex), m_Volume(volume), m_Inherited(inherited)
	{
	}

	} // namespace Beatmap
} // namespace Parser