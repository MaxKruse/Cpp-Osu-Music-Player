#pragma once
#include "Core.h"
#include "Beatmap/Beatmap.h"

#include "Beatmap/Hitcircle.h"
#include "Beatmap/Slider.h"
#include "Beatmap/Spinner.h"

namespace Parser {

	// Parser Class to handle .osu files
	class API_PLAYER Parser
	{
	public:
		Parser(const std::string SongsFolderPath, const std::string hitsoundsFolder, bool GetListOfFiles = true);
		Parser(const Parser& p) : m_SongsFolder(p.m_SongsFolder), m_HitsoundsFolder(p.m_HitsoundsFolder) {}

		std::unique_ptr<Beatmap::Beatmap> BeatmapFromFile(const std::string & FilePath);
		std::unique_ptr<Beatmap::Beatmap> BeatmapFromString(const std::vector<std::string> & Text);

		void GetAllFiles();

		const inline std::string const GetFullFilePath() const { return m_FullFilePath; }
		const inline std::string const GetFolderPath() const { return m_SongsFolder; }
		const inline std::string const GetFilePath() const { return m_FullFilePath; }
		const inline std::string const GetFileName() const { return m_FileName; }
		std::vector<std::string> const GetText() const { return m_Text; }
		std::vector<std::string> const GetListOfFiles() const { return m_ListOfFiles; };

	private:
		std::string m_SongsFolder;
		std::string m_FullFilePath;
		std::string m_FileName;
		std::string m_HitsoundsFolder;
		std::vector<std::string> m_Text;

		std::vector<std::string> m_ListOfFiles;

		std::ifstream m_FileHandle;
		
		std::vector<Beatmap::TimingPoint> timings;
		std::vector<std::shared_ptr<Beatmap::Hitobject>> hitobjects;

		Beatmap::General general;
		Beatmap::Metadata meta;
		Beatmap::SearchBy search;
		Beatmap::Difficulty diff;

	private:
		std::string ParseBackgroundImage();
		void		ParseHitobjects();
		void		ParseTimingPoints();

		std::string m_TempVersion;

		Beatmap::General    ParseGeneral();
		Beatmap::Metadata   ParseMetadata();
		Beatmap::SearchBy   ParseSearchBy();
		Beatmap::Difficulty ParseDifficulty();

		int CacheBeatmaps();

		std::string GetFolder();
		std::vector<std::string> FileToStringVector(const std::string & filename);

		// Taken from: https://stackoverflow.com/a/46931770
		std::vector<std::string> split(const std::string & s, const char & delim) {
			std::vector<std::string> result;
			std::stringstream ss(s);
			std::string item;

			while (std::getline(ss, item, delim)) {
				result.push_back(item);
			}

			return result;
		}
	};

}  // namespace Parser