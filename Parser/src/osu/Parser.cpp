#include "pch.h"
#include "Parser.h"

namespace Parser {

	Parser::Parser(const std::string & SongsFolderPath, bool GetListOfFiles)
		: m_SongsFolder(SongsFolderPath)
	{
		LOGGER_INFO("Songs folder set => {}", m_SongsFolder);

		GetAllFiles();
	}

	std::shared_ptr<Beatmap> Parser::BeatmapFromFile(const std::string & FilePath)
	{
		LOGGER_DEBUG("Parsing from file => {}", FilePath);
		auto Fullpath = std::string(m_SongsFolder + FilePath);
		m_FullFilePath = Fullpath;

		m_FileName = FilePath;
				
		m_Text = FileToStringVector(m_FullFilePath);

		LOGGER_DEBUG("Parsing Background Image from file => {}", m_FullFilePath);
		auto image = ParseBackgroundImage();
		LOGGER_DEBUG("Parsing General from file => {}", m_FullFilePath);
		General general = ParseGeneral();
		LOGGER_DEBUG("Parsing Metadata from file => {}", m_FullFilePath);
		Metadata meta = ParseMetadata();
		LOGGER_DEBUG("Parsing SearchBy from file => {}", m_FullFilePath);
		SearchBy search = ParseSearchBy();
		LOGGER_DEBUG("Parsing Difficulty from file => {}", m_FullFilePath);
		Difficulty diff = ParseDifficulty();
		// TODO: If any of the above contains default values (-1 for everything except Difficulty diff), throw an error @done(2019-01-02 18:46 UTC+01)
		if (general.HasDefaults() || meta.HasDefaults() || search.HasDefaults() || diff.HasDefaults())
		{
			LOGGER_DEBUG("Headerinformation (General, Metadata, SearchBy, Difficulty) incomplete, make sure your files have the correct format. File => {}", FilePath);
		}

		// TODO: Parsing TimingsPoints, Then Hitobjects @done(2019-01-02 18:22 UTC+01)
		LOGGER_DEBUG("Parsing TimingsPoints from file => {}", m_FullFilePath);
		timings = ParseTimingPoints();
		LOGGER_DEBUG("Parsing Hitobjects from file => {}", m_FullFilePath);
		hitobjects = ParseHitobjects();

		m_Text = std::vector<std::string>(500);

		return std::make_shared<Beatmap>(m_FullFilePath, image, hitobjects, timings, general, meta, search, diff);
	}

	std::shared_ptr<Beatmap> Parser::BeatmapFromString(const std::vector<std::string> & Text)
	{
		LOGGER_DEBUG("Parsing from StringVector");
		m_Text = Text;
		m_FullFilePath = "NO PATH GIVEN";
		m_FileName = "NO FILE GIVEN";
		return std::make_shared<Beatmap>();
	}

	void Parser::GetAllFiles()
	{
		if (m_ListOfFiles.size() < 1)
		{
			auto count = CacheBeatmaps();
			LOGGER_INFO("Cached {} Beatmaps", count);
			return;
		}
	}

	int Parser::CacheBeatmaps()
	{
		std::ifstream readFile;
		readFile.open("Beatmaps.prs");
		std::string line;

		if (m_ListOfFiles.size() > 1)
		{
			return m_ListOfFiles.size();
		}

		if (!readFile.is_open())
		{
			LOGGER_INFO("Couldn't open => {}", "Beatmaps.prs");
			LOGGER_INFO("Creating now...");
		}
		else
		{
			int l = 0;
			while (std::getline(readFile, line))
			{
				if (l == 0)
				{
					auto path = line.erase(0, 15);
					if (path != m_SongsFolder)
					{
						LOGGER_INFO("Couldn't find cache for folder => {}", m_SongsFolder);
						LOGGER_INFO("Creating now...");
						readFile.close();
						DeleteFile(L"Beatmaps.prs");
						break;
					}
				}

				if (line.length() != 0 && line.find("cached_path_is=") == std::string::npos)
				{
					m_ListOfFiles.push_back(line);
				}

				l++;
			}
			if (l != 0)
			{
				return m_ListOfFiles.size();
			}
		}

		readFile.close();
		
		std::ofstream writeFile;
		writeFile.open("Beatmaps.prs");
		writeFile.write("cached_path_is=", 15);
		writeFile.write(m_SongsFolder.c_str(), strlen(m_SongsFolder.c_str()));
		writeFile.write("\n", 1);

		for (std::experimental::filesystem::recursive_directory_iterator i(m_SongsFolder), end; i != end; ++i)
		{
			if (!is_directory(i->path()))
			{
				LOGGER_TRACE("Element => {}", i->path().string());
				// See: https://stackoverflow.com/a/23658737
				if (i->path().extension() == ".osu")
				{
					LOGGER_TRACE("Found file => {}", i->path().string());
					std::string RelativeFilePath(i->path().string().erase(0, m_SongsFolder.size()));
					m_ListOfFiles.emplace_back(RelativeFilePath);
					writeFile.write(RelativeFilePath.c_str(), strlen(RelativeFilePath.c_str()));
					writeFile.write("\n", 1);
					writeFile.flush();
				}
			}
		}

		writeFile.flush();
		writeFile.close();
		LOGGER_INFO("Cached all beatmaps");
		return 0;
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

	std::vector<TimingPoint> Parser::ParseTimingPoints()
	{
		std::vector<TimingPoint> timingpoints;

		if (m_TempVersion != "v14" && m_TempVersion != "v13" && m_TempVersion != "v12")
		{
			LOGGER_WARN("File Version too old. Cant parse timings and Hitobjects.");
			return timingpoints;
		}

		bool timingpoints_start = false;
		size_t counter = 0;

		// Get Amount of Timingpoints to resize vector
		for (auto& line : m_Text)
		{

			if (line.find("[TimingPoints]") != std::string::npos)
			{
				timingpoints_start = true;
				continue;
			}

			if (timingpoints_start && line.length() <= 15) // There is 2 Empty lines after the last timingpoint
			{
				timingpoints_start = false;
				break;
			}

			if (timingpoints_start && line.length() >= 15)
			{
				counter++;
			}
		}

		timingpoints.reserve(counter);

		for (auto& line : m_Text)
		{
			if (line.find("[TimingPoints]") != std::string::npos)
			{
				LOGGER_TRACE("TIMINGPOINTS LOCATED");
				timingpoints_start = true;
				continue;
			}

			if (line.length() <= 15 && timingpoints_start) // There is 2 Empty lines after the last timingpoint
			{
				timingpoints_start = false;
				break;
			}

			if (timingpoints_start)
			{
				LOGGER_TRACE("FOUND TIMINGPOINT => {}", line);
				LOGGER_TRACE("Parsing Now...");

				std::vector<std::string> parts = split(line, ',');

				long offset = stol(parts[0]);
				unsigned short milliseconds_per_beat = stoi(parts[1]);
				unsigned short sampleset = stoi(parts[3]);
				unsigned short sampleindex = stoi(parts[4]);
				unsigned short volume = stoi(parts[5]);
				
				bool inherited = false;

				if (stoi(parts[6]) == 1)
					inherited = true;

				timingpoints.push_back(TimingPoint(offset, milliseconds_per_beat, sampleset, sampleindex, volume, inherited));
			}

		}

		return timingpoints;
	}

	std::vector<Hitobject*> Parser::ParseHitobjects()
	{
		std::vector<Hitobject*> hitobjects;

		if (m_TempVersion != "v14" && m_TempVersion != "v13" && m_TempVersion != "v12")
		{
			LOGGER_WARN("File Version too old. Cant parse timings and Hitobjects.");
			return hitobjects;
		}

		bool hitobjects_start = false;
		size_t counter = 0;

		// Get Amount of Hitobjects to resize vector
		for (auto& line : m_Text)
		{

			if (line.find("[HitObjects]") != std::string::npos)
			{
				hitobjects_start = true;
				continue;
			}

			if (hitobjects_start && line.length() >= 16)
			{
				counter++;
			}
		}

		hitobjects_start = false;

		hitobjects.reserve(counter);

		// actually fill the thing
		for (auto& line : m_Text)
		{

			if (line.find("[HitObjects]") != std::string::npos)
			{
				hitobjects_start = true;
				continue;
			}

			if (hitobjects_start && line.length() >= 16)
			{
				LOGGER_TRACE("FOUND HITOBJECT => {}", line);
				LOGGER_TRACE("Parsing Now...");

				std::vector<std::string> parts = split(line, ',');
				
				if (stoi(parts[3]) & 1) // Circle
				{
					std::vector<std::string> extras = split(parts[5], ':');
					hitobjects.emplace_back(new Hitcircle(stoi(parts[0]), stoi(parts[1]), stol(parts[2]), stoi(parts[3]), stoi(parts[4]), extras));
					
				}
				else if (stoi(parts[3]) & 2) // Slider
				{
					// TODO: IMPLEMENT SLIDER
					
				}
				else if (stoi(parts[3]) & 8)  // Spinner
				{
					// TODO: IMPLEMENT SPINNER
					
				}
				else // Invalid Object
				{
					LOGGER_ERROR("Hitobject not supported => {}", line);
				}

			}
		}
		
		return hitobjects;
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
				m_TempVersion = FileFormatVersion;
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

		LOGGER_DEBUG("Reading file => {}", filename);

		m_FileHandle.open(filename);

		if (!m_FileHandle.is_open())
		{
			LOGGER_ERROR("Couldn't open => {}", filename);
			LOGGER_ERROR("Are you sure the file exists and isnt being used by any other program?");
			return result;
		}

		while (std::getline(m_FileHandle, line))
		{
			result.push_back(line);
		}

		LOGGER_DEBUG("Closing file => {}", filename);
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