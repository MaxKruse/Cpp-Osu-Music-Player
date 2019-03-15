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

		void Hitsound::AddStream(std::string filename, HSTREAM stream)
		{
			m_SampleStreams.emplace(std::pair<std::string, HSTREAM>(filename, stream));
		}

		void Hitsound::Play()
		{
			for (const auto& pair : m_SampleStreams)
			{
				LOGGER_DEBUG("Playing {} at {}ms with {}% Volume", pair.first, m_Offset, m_Volume);
				BASS_ChannelPlay(pair.second, true);
			}	
		}

		void Hitsound::ChangePlaybackVolume(float vol)
		{
			for (const auto& pair : m_SampleStreams)
			{
				BASS_ChannelSetAttribute(pair.second, BASS_ATTRIB_VOL, vol);
			}
		}

	} // namespace Beatmap
}  // namespace Parser