#pragma once
#include "pch.h"
#include "Hitobject.h"

namespace Parser {

	class Beatmap
	{
	public:
		Beatmap();
		~Beatmap();
	private:
		std::wstring m_DirectoryPath;

		std::vector<Hitobject> m_HitObjects;


		/// <summary>
		/// General data (not categorized) for the individual .osu file
		/// </summary>
		struct General
		{
			std::string  m_FileFormatVersion;
			std::string  m_AudioFilename;
			std::string  m_SampleSet;
			uint8_t      m_Mode;
		};

		/// <summary>
		/// Metadata Stored for the individual .osu file
		/// </summary>
		struct Metadata
		{
			std::string  m_Title;
			std::wstring m_UnicodeTitle;
			std::string  m_Artist;
			std::wstring m_ArtistUnicode;
			std::string  m_Creator;
			std::wstring m_Version;
		};
		

		/// <summary>
		/// These attributes can be used for search algorithms
		/// </summary>
		struct SearchBy
		{

			std::string  m_Source;
			std::string  m_Tags;
			int          m_BeatmapID;
			int          m_BeatmapSetID;
		};


		/// <summary>
		/// Values inside the Difficulty Struct are stored as unsigned shorts to save memory.
		/// To use these as floats, divide them by 10.
		/// 
		/// Example:
		/// 
		/// m_CircleSize = 40;
		/// float TrueCircleSize = m_CircleSize / 10.0f;
		/// 
		/// </summary>
		struct Difficulty
		{
			unsigned short m_HPDrainRate;
			unsigned short m_CircleSize;
			unsigned short m_OverallDifficulty;
			unsigned short m_ApproachRate;
		};

	};

}