#pragma once
#include "pch.h"
#include "Beatmap.h"

namespace Parser {

	class Parser
	{
	public:
		Parser(const std::string& SongsFolderPath);

		Beatmap BeatmapFromFile(const std::string & FilePath);
		Beatmap BeatmapFromString(const std::vector<std::string> & Text);

		inline std::string const GetFilePath() const { return m_FullFilePath; }
		inline std::string const GetFileName() const { return m_FileName; }
		std::vector<std::string> const GetText() const { return m_Text; }
	private:
		std::string m_SongsFolder;
		std::string m_FullFilePath;
		std::string m_FileName;
		std::vector<std::string> m_Text;

		std::ifstream m_FileHandle;

	private:
		std::string ParseBackgroundImage();
		std::vector<Hitobject> ParseHitobjects();
		std::vector<TimingPoint> ParseTimingPoints();

		General    ParseGeneral();
		Metadata   ParseMetadata();
		SearchBy   ParseSearchBy();
		Difficulty ParseDifficulty();

		std::vector<std::string> FileToStringVector(std::string filename);
	};

}