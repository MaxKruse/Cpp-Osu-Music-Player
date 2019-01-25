#pragma once
#include "Core.h"
#include "Logger.h"
#include "Hitobject.h"
#include "TimingPoint.h"

namespace Parser {

	/// <summary>
	/// General data, otherwise not categorized
	/// </summary>
	class API General
	{
		std::string m_FileFormatVersion;
		std::string m_AudioFilename;
		std::string m_SampleSet;
		int      m_Mode{-1};
	public:
		General() 
		{
			LOGGER_WARN("General cant be set!");
		}

		General(std::string FileFormatVersion, std::string AudioFilename, std::string SampleSet, int Mode)
			: m_FileFormatVersion(std::move(FileFormatVersion)), m_AudioFilename(std::move(AudioFilename)), m_SampleSet(std::move(SampleSet)), m_Mode(Mode)
		{
			LOGGER_TRACE("{}", ToString());
		}

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "General => [FileFormatVersion = " << m_FileFormatVersion << ", AudioFilename = " << m_AudioFilename
			<< ", SampleSet = " << m_SampleSet << ", Mode = " << m_Mode << "]";
			return ss.str();
		}

		inline const std::string GetAudioFilename() const { return m_AudioFilename; }
		inline const std::string GetFileformatVersion() const { return m_FileFormatVersion; }
		inline const int GetMode() const { return m_Mode; }

		bool HasDefaults()
		{
			if (m_FileFormatVersion.empty() || m_AudioFilename.empty() || m_SampleSet.empty() || m_Mode == -1)
			{
				return true;
			}
			return false;
		}

	};

	/// <summary>
	/// Metadata Stored for the individual .osu file
	/// </summary>
	class API Metadata
	{
		std::string m_Artist;
		std::string m_ArtistUnicode;
		std::string m_Title;
		std::string m_TitleUnicode;
		std::string m_Creator;
		std::string m_Version;
	public:
		Metadata()
		{
			LOGGER_WARN("Metadata cant be set!");
		}

		Metadata(std::string Artist, std::string ArtistUnicode, std::string Title, std::string TitleUnicode, std::string Creator, std::string Version)
			: m_Artist(std::move(Artist)), m_ArtistUnicode(std::move(ArtistUnicode)), m_Title(std::move(Title)), m_TitleUnicode(std::move(TitleUnicode)), m_Creator(std::move(Creator)), m_Version(std::move(Version))
		{
			LOGGER_TRACE("{}", ToString());
		}

		inline const std::string GetArtist() const { return m_Artist; }
		inline const std::string GetTitle() const { return m_Title; }
		inline const std::string GetCreator() const { return m_Creator; }
		inline const std::string GetVersion() const { return m_Version; }

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "Metadata => [Arist = " << m_Artist << ", ArtistUnicode = " << m_ArtistUnicode 
			<< ", Title = " << m_Title << ", TitleUnicode = " << m_TitleUnicode
			<< ", Creator = " << m_Creator << ", Version = " << m_Version << "]";
			return ss.str();
		}

		bool HasDefaults()
		{
			if (m_Artist.empty() || m_ArtistUnicode.empty() || m_Title.empty() || m_TitleUnicode.empty() || m_Creator.empty() || m_Version.empty())
			{
				return true;
			}
			return false;
		}

	};

	/// <summary>
	/// These attributes can be used for search algorithms
	/// </summary>
	class API SearchBy
	{
		std::string m_Source;
		std::string m_Tags;
		int         m_BeatmapID{-1};
		int         m_BeatmapSetID{-1};
	public:
		SearchBy()
		{
			LOGGER_WARN("SearchBy cant be set!");
		}

		SearchBy(std::string Source, std::string Tags, int BeatmapID, int BeatmapSetID)
			: m_Source(std::move(Source)), m_Tags(std::move(Tags)), m_BeatmapID(BeatmapID), m_BeatmapSetID(BeatmapSetID)
		{
			LOGGER_TRACE("{}", ToString());
		}

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "SearchBy => [Source = " << m_Source << ", Tags = " << m_Tags
			<< ", BeatmapID = " << m_BeatmapID << ", BeatmapSetID = " << m_BeatmapSetID << "]";
			return ss.str();
		}

		bool HasDefaults()
		{
			if (m_Source.empty() || m_Tags.empty() || m_BeatmapID == -1 || m_BeatmapSetID == -1)
			{
				return true;
			}
			return false;
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
	class API Difficulty
	{
		unsigned short m_HPDrainRate{255};
		unsigned short m_CircleSize{255};
		unsigned short m_OverallDifficulty{255};
		unsigned short m_ApproachRate{255};
		float m_SliderMultiplier{-1.0f};
		unsigned short m_SliderTickRate{255};
	public:
		Difficulty()
		{
			LOGGER_WARN("Difficulty cant be set!");
		}

		Difficulty(unsigned short HPDrainRate, unsigned short CircleSize, unsigned short OverallDifficulty, unsigned short ApproachRate, float SliderMultiplier, unsigned short SliderTickRate)
			: m_HPDrainRate(HPDrainRate), m_CircleSize(CircleSize), m_OverallDifficulty(OverallDifficulty), m_ApproachRate(ApproachRate), m_SliderMultiplier(SliderMultiplier), m_SliderTickRate(SliderTickRate)
		{
			LOGGER_TRACE("{}", ToString());
		}

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "Difficulty => [HP = " << m_HPDrainRate / 10.0 << ", CS = " << m_CircleSize / 10.0
			<< ", OD = " << m_OverallDifficulty / 10.0 << ", AR = " << m_ApproachRate / 10.0 << "]";
			return ss.str();
		}

		bool HasDefaults()
		{
			if (m_HPDrainRate == 255 || m_ApproachRate == 255 || m_CircleSize == 255 || m_OverallDifficulty == 255 || m_SliderMultiplier == -1.0f || m_SliderTickRate == 255)
			{
				return true;
			}
			return false;
		}
	};

	// A Beatmap is a single Object that knows about all of its contents (see .osu files for contents)
	class API Beatmap
	{
	public:
		Beatmap();

		Beatmap(const std::string & FilePath, const std::string & Folder, const std::string & BackgroundImage, std::vector<Hitobject*> Hitobjects, std::vector<TimingPoint> Timingpoints, General g, Metadata m, SearchBy s, Difficulty d);
		~Beatmap();

		inline const std::map<long, std::vector<std::string>> GetHitsoundsOfTimings() const { return m_HitsoundsOnTiming; }
		const long GetLastOffset() const
		{
			auto last_object = m_HitObjects.size() - 1;
			auto offsets_of_last = m_HitObjects[last_object]->GetOffsets();
			return offsets_of_last[offsets_of_last.size() - 1];
		}

		inline const std::string GetMetadataText() const { return std::string(m_Metadata.GetArtist() + " - " + m_Metadata.GetTitle() + " (" + m_Metadata.GetCreator() + ") [" + m_Metadata.GetVersion() + "]"); }
		inline const std::string GetMp3() const { return m_General.GetAudioFilename(); }
		inline const std::string GetFolderPath() const { return m_Folder; }
		inline const std::string GetFullMp3Path() const { return GetFolderPath() + GetMp3(); }
		inline const std::string GetFilePath() const { return m_FilePath; }
		inline const std::vector<std::vector<long>> GetOffsets() const { return m_Offsets; }
		const double GetBPM() const // This calculation is wrong, only correct up to the decimal point (+- 1)
		{
			return (60000.0 / m_TimingPoints[0].GetMillisecondsPerBeat());
		}

		inline const bool IsPlayable() const // Only make supported files playable
		{ 
			if (m_General.GetFileformatVersion() == "v13" && m_General.GetMode() == 0)
			{
				return true;
			}
			
			if (m_General.GetFileformatVersion() == "v14" && m_General.GetMode() == 0)
			{
				return true;
			}

			return false;
		}

		std::string ToString() const;
		
	private:
		std::string                           m_FilePath;
		std::string                           m_Folder;
		std::string                           m_BackgroundImage;
		std::vector<Hitobject*>               m_HitObjects;
		std::vector<TimingPoint>              m_TimingPoints;
		std::vector<std::vector<long>>		  m_Offsets;

		std::map<long, std::vector<std::string>> m_HitsoundsOnTiming;
		
		General    m_General;
		Metadata   m_Metadata;
		SearchBy   m_SearchBy;
		Difficulty m_Difficulty;
	};

	// These are operators for the Logger to use, they tell it how to insert objects and print their info (see the Class' ".ToString()" function for more info)

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

	inline std::ostream& operator<<(std::ostream& os, const std::unique_ptr<Beatmap>& b)
	{
		return os << b->ToString();
	}
}  // namespace Parser