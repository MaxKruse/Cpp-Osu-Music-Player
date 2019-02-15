#pragma once
#include "Core.h"
#include "Beatmap.h"

#include "Hitcircle.h"
#include "Slider.h"
#include "Spinner.h"

namespace Parser {
	time_t x;

	// Parser Class to handle .osu files
	class API Parser
	{
	public:
		Parser(const std::string SongsFolderPath, bool GetListOfFiles = true);

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

		General general;
		Metadata meta;
		SearchBy search;
		Difficulty diff;

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

		std::vector<std::string> FileToStringVector(const std::string & filename);
		std::vector<std::string> split(const std::string & s, const char & delim);
	};


	// Random Functions using Low/High or a vector of sorts, to return a random index between given low and high
	size_t Random(size_t low = 0, size_t high = 0xFFFFFFF)
	{
		size_t z = rand() % (high - low);
		z += low;
		return z;
	}

	template<typename T>
	size_t Random(std::vector<T> input)
	{
		if (input.size() == 1)
		{
			return 0;
		}
		size_t low = 0;
		size_t high = input.size();
		size_t z = rand() % (high - low);
		z += low - 1;
		return z;
	}

	// See: https://stackoverflow.com/a/3418285
	void replaceAll(std::string& str, const std::string& from, const std::string& to) {
		if (from.empty())
		{
			return;
		}
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}

}  // namespace Parser