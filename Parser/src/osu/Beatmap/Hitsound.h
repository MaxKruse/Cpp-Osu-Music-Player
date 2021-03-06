#pragma once
#include "Logger.h"
#include "bass.h"

namespace Parser {
	namespace Beatmap {

		/*
			Bit 0 (1): normal.
			Bit 1 (2): whistle.
			Bit 2 (4): finish.
			Bit 3 (8): clap.
		*/
		enum HITSOUND_ADDITION
		{
			HITSOUND_NORMAL = 1 << 0,
			HITSOUND_WHISTLE = 1 << 1,
			HITSOUND_FINISH = 1 << 2,
			HITSOUND_CLAP = 1 << 3,

		};

		enum HITSOUND_SET
		{
			SET_AUTO = 0,
			SET_NORMAL = 1,
			SET_SOFT = 2,
			SET_DRUM = 3
		};

		class Hitsound
		{
		public:
			Hitsound();
			Hitsound(long offset, std::vector<std::string> sampleFiles, unsigned char volume);
			~Hitsound();

			inline const std::vector<std::string> GetSampleNames() const { return m_SampleFiles; }
			inline const long GetOffset() const { return m_Offset; }
			inline const unsigned char GetVolume() const { return m_Volume; }

			void AddStream(std::string, HSTREAM);
			void Play();
			void ChangePlaybackVolume(float);

		private:
			long m_Offset;
			unsigned char m_Volume;
			std::vector<std::string> m_SampleFiles;
			std::map<std::string, HSTREAM> m_SampleStreams;

			bool operator==(const Hitsound* other)
			{
				return (GetOffset() == other->GetOffset() && GetVolume() == other->GetVolume() && GetSampleNames() == other->GetSampleNames());
			}
		};

		

	} // namespace Beatmap
}  // namespace Parser
