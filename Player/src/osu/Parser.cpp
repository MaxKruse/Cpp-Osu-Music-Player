#include "pch.h"
#include "Parser.h"

namespace Parser {

	Parser::Parser(const std::string & SongsFolderPath)
		: m_SongsFolder(SongsFolderPath)
	{
		LOGGER_INFO("Songs Folder Set: {}", m_SongsFolder);
	}

	Beatmap Parser::BeatmapFromFile(const std::string & FilePath)
	{
		LOGGER_INFO("Parsing from File {}", FilePath);
		return Beatmap("Test", std::vector<Hitobject>{ Hitobject() }, General("4", "3", "2", 1), Metadata("5","6","7","8","9","10"), SearchBy("11","12",13, 14), Difficulty(150, 160, 170, 180));
	}

	Beatmap Parser::BeatmapFromString(const std::string & Text)
	{
		LOGGER_INFO("Parsing from String {}", Text);
		return Beatmap();
	}

}