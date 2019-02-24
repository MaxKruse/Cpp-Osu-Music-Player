#include "pch.h"
#include "Hitsound.h"
namespace Parser {
	namespace Beatmap {

		Hitsound::Hitsound(long & offset, std::vector<std::string> sampleFiles)
			: m_Offset(offset), m_SampleFiles(std::move(sampleFiles))
		{
		}


		Hitsound::~Hitsound()
		{
			m_SampleFiles.clear();
			LOGGER_DEBUG("Hitsound Destroyed");
		}

	} // namespace Beatmap
}  // namespace Parser