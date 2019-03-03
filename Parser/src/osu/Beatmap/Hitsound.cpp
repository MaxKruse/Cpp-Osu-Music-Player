#include "pch.h"
#include "Hitsound.h"

namespace Parser {
	namespace Beatmap {

		Hitsound::Hitsound(long offset, std::vector<std::string> sampleFiles, unsigned char volume)
			: m_Offset(offset), m_SampleFiles(std::move(sampleFiles)), m_Volume(volume)
		{
		}

		Hitsound::~Hitsound()
		{
		}

		void Hitsound::AddStream(HSTREAM stream)
		{
			m_SampleStreams.emplace_back(stream);
		}

		void Hitsound::Play()
		{
			for (const auto& stream : m_SampleStreams)
			{
				LOGGER_DEBUG("Playing at {}ms with {}% Volume", m_Offset, m_Volume);
				BASS_ChannelPlay(stream, true);
			}	
		}

		void Hitsound::ChangePlaybackVolume(float vol)
		{
			for (const auto& stream : m_SampleStreams)
			{
				BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, vol);
			}
		}

	} // namespace Beatmap
}  // namespace Parser