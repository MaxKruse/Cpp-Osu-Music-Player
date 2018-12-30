#include "pch.h"
#include "Parser.h"

namespace Parser {

	Parser::Parser(const std::string & SongsFolderPath)
		: m_SongsFolder(SongsFolderPath)
	{
		LOGGER_INFO("Songs folder set => {}", m_SongsFolder);
	}

	Beatmap Parser::BeatmapFromFile(const std::string & FilePath)
	{
		LOGGER_INFO("Parsing from file => {}", FilePath);
		std::string Fullpath = std::string(m_SongsFolder + FilePath);
		m_FullFilePath = Fullpath;

		m_FileName = FilePath;
		
		m_Text = FileToStringVector(m_FullFilePath);

		LOGGER_INFO("Parsing Background Image from file => {}", m_FullFilePath);
		std::string image = ParseBackgroundImage();
		LOGGER_INFO("Parsing General from file => {}", m_FullFilePath);
		General general = ParseGeneral();
		LOGGER_INFO("Parsing Metadata from file => {}", m_FullFilePath);
		Metadata meta = ParseMetadata();
		LOGGER_INFO("Parsing SearchBy from file => {}", m_FullFilePath);
		SearchBy search = ParseSearchBy();
		LOGGER_INFO("Parsing Difficulty from file => {}", m_FullFilePath);
		Difficulty diff = ParseDifficulty();

		// TODO: If any of the above contains default values (-1 for everything except Difficulty diff), throw an error

		// TODO: Parsing TimingsPoints, Then Hitobjects
		LOGGER_INFO("Parsing TimingsPoints from file => {}", m_FullFilePath);
		std::vector<TimingPoint> timings = ParseTimingPoints();
		LOGGER_INFO("Parsing Hitobjects from file => {}", m_FullFilePath);
		std::vector<Hitobject> hitobjects = ParseHitobjects();

		return Beatmap(m_FullFilePath, image, hitobjects, timings, general, meta, search, diff);
	}

	Beatmap Parser::BeatmapFromString(const std::vector<std::string> & Text)
	{
		LOGGER_INFO("Parsing from StringVector");
		m_Text = Text;
		m_FullFilePath = "NO PATH GIVEN";
		m_FileName = "NO FILE GIVEN";
		return Beatmap();
	}

	std::string Parser::ParseBackgroundImage()
	{
		std::string image = "-1";
		bool next_is_image = false;
		for (auto& line : m_Text)
		{
			if (next_is_image)
			{
				image = line.erase(0, 5);
				image = image.erase(image.length() - 5, 5);
				LOGGER_TRACE("FOUND BACKGROUND IMAGE => {}", image);
				break;
			}

			if (line.find("//Background and Video events") != std::string::npos)
			{
				next_is_image = true;
				continue;
			}
		}

		return image;
	}

	std::vector<Hitobject> Parser::ParseHitobjects()
	{
		std::vector<Hitobject> hitobjects;

		bool hitobjects_start = false;
		for (auto& line : m_Text)
		{
			if (hitobjects_start)
			{
				LOGGER_TRACE("FOUND HITOBJECT => {}", line);
				LOGGER_TRACE("Parsing Now...");

				std::vector<std::string> parts = split(line, ',');
				
			}

			if (line.find("[HitObjects]") != std::string::npos)
			{
				hitobjects_start = true;
				continue;
			}
		}
		
		return hitobjects;
	}

	std::vector<TimingPoint> Parser::ParseTimingPoints()
	{
		return std::vector<TimingPoint>();
	}

	General Parser::ParseGeneral()
	{
		std::string FileFormatVersion = "-1";
		std::string AudioFilename = "-1";
		std::string SampleSet = "-1";
		int Mode = -1;

		int found = 0;

		for(auto line : m_Text)
		{

			if (found == 4)
				break;
			
			if (line.find("osu file format v") != std::string::npos)
			{
				FileFormatVersion = line.erase(0, 16);
				LOGGER_TRACE("FOUND FILE FORMAT => {}", FileFormatVersion);
				found++;
			}

			if (line.find("AudioFilename: ") != std::string::npos)
			{
				AudioFilename = line.erase(0, 15);
				LOGGER_TRACE("FOUND AUDIOFILENAME => {}", AudioFilename);
				found++;
			}

			if (line.find("SampleSet: ") != std::string::npos)
			{
				SampleSet = line.erase(0, 11);
				LOGGER_TRACE("FOUND SAMPLESET => {}", SampleSet);
				found++;
			}

			if (line.find("Mode: ") != std::string::npos)
			{
				Mode = atoi(line.erase(0, 6).c_str());
				LOGGER_TRACE("FOUND MODE => {}", Mode);
				found++;
			}
		}

		return General(FileFormatVersion, AudioFilename, SampleSet, Mode);
	}

	Metadata Parser::ParseMetadata()
	{
		std::string Artist = "-1";
		std::string ArtistUnicode = "-1";
		std::string Title = "-1";
		std::string TitleUnicode = "-1";
		std::string Creator = "-1";
		std::string Version = "-1";

		int found = 0;

		for (auto line : m_Text)
		{

			if (found == 6)
				break;

			if (line.find("Artist:") != std::string::npos)
			{
				Artist = line.erase(0, 7);
				LOGGER_TRACE("FOUND ARTIST => {}", Artist);
				found++;
			}

			if (line.find("ArtistUnicode:") != std::string::npos)
			{
				ArtistUnicode = line.erase(0, 14);
				LOGGER_TRACE("FOUND ARTISTUNICODE => {}", ArtistUnicode);
				found++;
			}

			if (line.find("Title:") != std::string::npos)
			{
				Title = line.erase(0, 6);
				LOGGER_TRACE("FOUND TITLE => {}", Title);
				found++;
			}

			if (line.find("TitleUnicode:") != std::string::npos)
			{
				TitleUnicode = line.erase(0, 13);
				LOGGER_TRACE("FOUND TITLEUNICODE => {}", TitleUnicode);
				found++;
			}

			if (line.find("Creator:") != std::string::npos)
			{
				Creator = line.erase(0, 8);
				LOGGER_TRACE("FOUND CREATOR => {}", Creator);
				found++;
			}

			if (line.find("Version:") != std::string::npos)
			{
				Version = line.erase(0, 8);
				LOGGER_TRACE("FOUND VERSION => {}", Version);
				found++;
			}
		}

		return Metadata(Artist, ArtistUnicode, Title, TitleUnicode, Creator, Version);
	}

	SearchBy Parser::ParseSearchBy()
	{
		std::string Source = "-1";
		std::string Tags = "-1";
		int         BeatmapID = -1;
		int         BeatmapSetID = -1;

		int found = 0;

		for (auto line : m_Text)
		{

			if (found == 4)
				break;

			if (line.find("Source:") != std::string::npos)
			{
				Source = line.erase(0, 7);
				LOGGER_TRACE("FOUND SOURCE => {}", Source);
				found++;
			}

			if (line.find("Tags:") != std::string::npos)
			{
				Tags = line.erase(0, 5);
				LOGGER_TRACE("FOUND TAGS => {}", Tags);
				found++;
			}

			if (line.find("BeatmapID:") != std::string::npos)
			{
				BeatmapID = atoi(line.erase(0, 10).c_str());
				LOGGER_TRACE("FOUND BEATMAPID => {}", BeatmapID);
				found++;
			}

			if (line.find("BeatmapSetID:") != std::string::npos)
			{
				BeatmapSetID = atoi(line.erase(0, 13).c_str());
				LOGGER_TRACE("FOUND BEATMAPSETID => {}", BeatmapSetID);
				found++;
			}
		}

		return SearchBy(Source, Tags, BeatmapID, BeatmapSetID);
	}

	Difficulty Parser::ParseDifficulty()
	{
		unsigned short HPDrainRate = 255;
		unsigned short CircleSize = 255;
		unsigned short OverallDifficulty = 255;
		unsigned short ApproachRate = 255;
		float SliderMultiplier = 255.0f;
		unsigned short SliderTickRate = 255;

		int found = 0;

		for (auto line : m_Text)
		{

			if (found == 6)
				break;

			if (line.find("HPDrainRate:") != std::string::npos)
			{
				HPDrainRate = (unsigned short)atoi(line.erase(0, 12).c_str()) * 10;
				LOGGER_TRACE("FOUND HPDRAINRATE => {}", HPDrainRate / 10.0);
				found++;
			}

			if (line.find("CircleSize:") != std::string::npos)
			{
				CircleSize = (unsigned short)atoi(line.erase(0, 11).c_str()) * 10;
				LOGGER_TRACE("FOUND CIRCLESIZE => {}", CircleSize / 10.0);
				found++;
			}

			if (line.find("OverallDifficulty:") != std::string::npos)
			{
				OverallDifficulty = (unsigned short)atoi(line.erase(0, 18).c_str()) * 10;
				LOGGER_TRACE("FOUND OVERALLDIFFICULTY => {}", OverallDifficulty / 10.0);
				found++;
			}

			if (line.find("ApproachRate:") != std::string::npos)
			{
				ApproachRate = (unsigned short)atoi(line.erase(0, 13).c_str()) * 10;
				LOGGER_TRACE("FOUND APPROACHRATE => {}", ApproachRate / 10.0);
				found++;
			}

			if (line.find("SliderMultiplier:") != std::string::npos)
			{
				SliderMultiplier = stof(line.erase(0, 17));
				LOGGER_TRACE("FOUND SLIDERMULTIPLIER => {}", SliderMultiplier);
				found++;
			}

			if (line.find("SliderTickRate:") != std::string::npos)
			{
				SliderTickRate = (unsigned short)atoi(line.erase(0, 15).c_str());
				LOGGER_TRACE("FOUND SLIDERTICKRATE => {}", SliderTickRate);
				found++;
			}
		}

		return Difficulty(HPDrainRate, CircleSize, OverallDifficulty, ApproachRate, SliderMultiplier, SliderTickRate);
	}

	std::vector<std::string> Parser::FileToStringVector(std::string filename)
	{
		std::vector<std::string> result = std::vector<std::string>();
		std::string line;

		LOGGER_INFO("Reading file => {}", filename);

		m_FileHandle.open(filename);

		if (!m_FileHandle.is_open())
		{
			LOGGER_ERROR("Couldnt open => {}", filename);
			LOGGER_ERROR("Are you sure the file exists and isnt being used by any other program?");
			return result;
		}

		while (std::getline(m_FileHandle, line))
		{
			result.push_back(line);
		}

		LOGGER_INFO("Closing file => {}", filename);
		m_FileHandle.close();

		return result;
	}
	
	// Taken from: https://stackoverflow.com/a/46931770
	std::vector<std::string> Parser::split(const std::string & s, const char & delim) {
		std::vector<std::string> result;
		std::stringstream ss(s);
		std::string item;

		while (std::getline(ss, item, delim)) {
			result.push_back(item);
		}

		return result;
	}
}