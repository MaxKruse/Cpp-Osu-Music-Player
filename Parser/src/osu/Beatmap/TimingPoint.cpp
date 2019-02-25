#include "pch.h"
#include "TimingPoint.h"

namespace Parser {
	namespace Beatmap {

	TimingPoint::TimingPoint(long offset, float milliseconds_per_beat, unsigned short sampleset, unsigned short sampleindex, unsigned short volume, bool inherited)
		: m_Offset(offset), m_MillisecondsPerBeat(milliseconds_per_beat), m_SampleSet(sampleset), m_SampleIndex(sampleindex), m_Volume(volume), m_Inherited(inherited)
	{
	}

	std::string TimingPoint::ToString() const
	{
		std::stringstream res;
		res << "Offset: " << m_Offset << "\n";
		res << "\tSampleSet: " << m_SampleSet << "\n";
		res << "\tSampleIndex: " << m_SampleIndex << "\n";
		res << "\tVolume: " << m_Volume << "\n";
		if (m_Inherited)
		{
			res << "\tBPM: " << GetBPM() << "\n";
		}
		else
		{
			res << "\tSV Modifier: " << GetSvMultiplier() << "\n";
		}

		return res.str();
	}

	} // namespace Beatmap
} // namespace Parser