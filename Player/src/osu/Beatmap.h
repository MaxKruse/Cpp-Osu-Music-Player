#pragma once
#include "pch.h"
#include "Logger.h"
#include "Hitobject.h"

namespace Parser {

	struct General
	{
		std::string  m_FileFormatVersion;
		std::string  m_AudioFilename;
		std::string  m_SampleSet;
		int      m_Mode;

		General()
		{
			LOGGER_WARN("General cant be set!");
		}

		General(std::string FileFormatVersion, std::string AudioFilename, std::string SampleSet, int Mode)
			: m_FileFormatVersion(FileFormatVersion), m_AudioFilename(AudioFilename), m_SampleSet(SampleSet), m_Mode(Mode)
		{
		}

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "General:    [FileFormatVersion = " << m_FileFormatVersion << ", AudioFilename = " << m_AudioFilename
			<< ",\n             SampleSet = " << m_SampleSet << ", Mode = " << m_Mode << "]";
			return ss.str();
		}
	};

	/// <summary>
	/// Metadata Stored for the individual .osu file
	/// </summary>
	struct Metadata
	{
		std::string  m_Artist;
		std::string m_ArtistUnicode;
		std::string  m_Title;
		std::string m_TitleUnicode;
		std::string  m_Creator;
		std::string m_Version;

		Metadata()
		{
			LOGGER_WARN("Metadata cant be set!");
		}

		Metadata(std::string Artist, std::string ArtistUnicode, std::string Title, std::string TitleUnicode, std::string Creator, std::string Version)
			: m_Artist(Artist), m_ArtistUnicode(ArtistUnicode), m_Title(Title), m_TitleUnicode(TitleUnicode), m_Creator(Creator), m_Version(Version)
		{
		}

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "SearchBy:   [Arist = " << m_Artist << ", ArtistUnicode = " << m_ArtistUnicode 
			<< ",\n             Title = " << m_Title << ", TitleUnicode = " << m_TitleUnicode
			<< ",\n             Creator = " << m_Creator << ", Version = " << m_Version << "]";
			return ss.str();
		}


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

		SearchBy()
		{
			LOGGER_WARN("SearchBy cant be set!");
		}

		SearchBy(std::string Source, std::string Tags, int BeatmapID, int BeatmapSetID)
			: m_Source(Source), m_Tags(Tags), m_BeatmapID(BeatmapID), m_BeatmapSetID(BeatmapSetID)
		{
		}

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "SearchBy:   [Source = " << m_Source << ", Tags = " << m_Tags
			<< ",\n             BeatmapID = " << m_BeatmapID << ", BeatmapSetID = " << m_BeatmapSetID << "]";
			return ss.str();
		}
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

		Difficulty()
		{
			LOGGER_WARN("Difficulty cant be set!");
		}

		Difficulty(unsigned short HPDrainRate, unsigned short CircleSize, unsigned short OverallDifficulty, unsigned short ApproachRate)
			: m_HPDrainRate(HPDrainRate), m_CircleSize(CircleSize), m_OverallDifficulty(OverallDifficulty), m_ApproachRate(ApproachRate)
		{
		}

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "Difficulty: [HP = " << m_HPDrainRate / 10.0 << ", CS = " << m_CircleSize / 10.0
			<< ",\n             OD = " << m_OverallDifficulty / 10.0 << ", AR = " << m_ApproachRate / 10.0 << "]";
			return ss.str();
		}
	};

	class Beatmap
	{
	public:
		Beatmap();

		Beatmap(const std::string & DirectoryPath, std::vector<Hitobject> Hitobjects, General& g, Metadata& m, SearchBy& s, Difficulty& d);
		~Beatmap();

	private:
		/// <summary>
		/// General data (not categorized) for the individual .osu file
		/// </summary>

	private:
		std::string m_DirectoryPath;
		std::vector<Hitobject> m_HitObjects;

		General m_General;
		Metadata m_Metadata;
		SearchBy m_SearchBy;
		Difficulty m_Difficulty;

	};

	inline std::ostream& operator<<(std::ostream& os, const General& e)
	{
		return os << e.ToString();
	}

	inline std::ostream& operator<<(std::ostream& os, const Metadata& m)
	{
		return os << m.ToString();
	}

	inline std::ostream& operator<<(std::ostream& os, const SearchBy& s)
	{
		return os << s.ToString();
	}

	inline std::ostream& operator<<(std::ostream& os, const Difficulty& d)
	{
		return os << d.ToString();
	}
}