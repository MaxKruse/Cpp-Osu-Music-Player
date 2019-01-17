#pragma once
#include "Core.h"
#include "Beatmap.h"
#include "Hitcircle.h"

namespace Parser {

	std::mt19937_64 p_RNG;

	class API Parser
	{
	public:
		Parser(const std::string & SongsFolderPath, bool GetListOfFiles = true);

		std::unique_ptr<Beatmap> BeatmapFromFile(const std::string & FilePath);
		std::unique_ptr<Beatmap> BeatmapFromString(const std::vector<std::string> & Text);

		void GetAllFiles();

		inline std::string const GetFilePath() const { return m_FullFilePath; }
		inline std::string const GetFileName() const { return m_FileName; }
		std::vector<std::string> const GetText() const { return m_Text; }
		std::vector<std::string> const GetListOfFiles() const { return m_ListOfFiles; };
	private:
		std::string m_SongsFolder;
		std::string m_FullFilePath;
		std::string m_FileName;
		std::vector<std::string> m_Text;

		std::vector<std::string> m_ListOfFiles;

		std::ifstream m_FileHandle;
		
		std::vector<TimingPoint> timings;
		std::vector<Hitobject*> hitobjects;

	private:
		std::string ParseBackgroundImage();
		std::vector<Hitobject*> ParseHitobjects();
		std::vector<TimingPoint> ParseTimingPoints();

		std::string m_TempVersion;

		General    ParseGeneral();
		Metadata   ParseMetadata();
		SearchBy   ParseSearchBy();
		Difficulty ParseDifficulty();

		int CacheBeatmaps();

		std::vector<std::string> FileToStringVector(std::string filename);
		std::vector<std::string> split(const std::string & s, const char & delim);
	};

	size_t Random(size_t low = 0, size_t high = 0xFFFFFFF)
	{
		std::uniform_int_distribution<size_t> GetRandom(low, high);

		size_t index = GetRandom(p_RNG);
		return index;
	}

	template<typename T>
	size_t Random(std::vector<T> input)
	{
		size_t high = input.size();
		size_t low = 0;
		std::uniform_int_distribution<size_t> GetRandom(low, high);

		size_t index = GetRandom(p_RNG);
		return index;
	}

}  // namespace Parser