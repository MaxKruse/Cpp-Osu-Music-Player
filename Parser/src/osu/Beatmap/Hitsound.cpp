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
			m_SampleFiles.clear();
			LOGGER_DEBUG("Hitsound Destroyed");
		}

		void Hitsound::Play()
		{
			std::string files;

			for (const auto& s : m_SampleFiles)
			{
				LOGGER_DEBUG("Playing {} at {}ms with {}% Volume", s, m_Offset, m_Volume);
			}
		}

	} // namespace Beatmap
}  // namespace Parser