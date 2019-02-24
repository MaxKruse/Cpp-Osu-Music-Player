#pragma once
#include "Logger.h"

namespace Parser {
	namespace Beatmap {

		class Hitsound
		{
		public:
			Hitsound(long offset, std::vector<std::string> sampleFiles, unsigned char volume);
			~Hitsound();

			inline const std::vector<std::string> GetSampleNames() const { return m_SampleFiles; }
			inline const long GetOffset() const { return m_Offset; }

			void Play();

		private:
			long m_Offset;
			unsigned char m_Volume;
			std::vector<std::string> m_SampleFiles;
		};
	} // namespace Beatmap
}  // namespace Parser
