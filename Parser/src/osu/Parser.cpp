#include "pch.h"
#include "Parser.h"

namespace Parser {

	Parser::Parser(const std::string SongsFolderPath, const std::string hitsoundsFolder, bool GetListOfFiles)
		: m_SongsFolder(std::move(SongsFolderPath)), m_HitsoundsFolder(hitsoundsFolder)
	{
		general = Beatmap::General();
		meta = Beatmap::Metadata();
		search = Beatmap::SearchBy();
		diff = Beatmap::Difficulty();
		LOGGER_INFO("Songs folder set => {}", m_SongsFolder);
		// Seed random
		srand(time(nullptr));
		// Cache all files
		GetAllFiles();
	}

	// Return unique_ptr for auto-deconstruction on out-of-scope
	std::unique_ptr<Beatmap::Beatmap> Parser::BeatmapFromFile(const std::string & FilePath)
	{
		// Setting the filename for debug purposes and formating it to only use /
		auto Fullpath = std::string(m_SongsFolder + FilePath);
		m_FullFilePath = Fullpath;
		replaceAll(m_FullFilePath, "\\", "/");
		LOGGER_DEBUG("Parsing from file => {}", m_FullFilePath);

		// Formating to get the folder name itself
		auto Folder = GetFolder();

		// Setting the Filename and the Text
		m_FileName = m_FullFilePath;
		// Reading the File into a vector to mostly access continuous memory, not harddrive
		m_Text = FileToStringVector(m_FullFilePath);

		// Parsing various information from the .osu file
		LOGGER_DEBUG("Parsing Background Image from file => {}", m_FullFilePath);
		auto image = ParseBackgroundImage();
		LOGGER_DEBUG("Parsing General from file => {}", m_FullFilePath);
		general = ParseGeneral();
		LOGGER_DEBUG("Parsing Metadata from file => {}", m_FullFilePath);
		meta = ParseMetadata();
		LOGGER_DEBUG("Parsing SearchBy from file => {}", m_FullFilePath);
		search = ParseSearchBy();
		LOGGER_DEBUG("Parsing Difficulty from file => {}", m_FullFilePath);
		diff = ParseDifficulty();
		// If anything didnt get set, log it  
		if (general.HasDefaults() || meta.HasDefaults() || search.HasDefaults() || diff.HasDefaults())
		{
			LOGGER_DEBUG("Headerinformation (General, Metadata, SearchBy, Difficulty) incomplete, make sure your files have the correct format. File => {}", FilePath);
		}

		// TODO: Parsing TimingsPoints, Then Hitobjects
		LOGGER_DEBUG("Parsing TimingsPoints from file => {}", m_FullFilePath);
		ParseTimingPoints();
		LOGGER_DEBUG("Parsing Hitobjects from file => {}", m_FullFilePath);
		ParseHitobjects();

		// Empty the Text for RAM-Usage purposes
		m_Text.clear();
		return std::make_unique<Beatmap::Beatmap>(m_FullFilePath, Folder, m_HitsoundsFolder, image, hitobjects, timings, general, meta, search, diff);
	}

	// This is intended to do the same thing as above, except just reading from a vector itself (API purposes for other programs)
	std::unique_ptr<Beatmap::Beatmap> Parser::BeatmapFromString(const std::vector<std::string> & Text)
	{
		LOGGER_DEBUG("Parsing from StringVector");
		m_Text = Text;
		m_FullFilePath = "NO PATH GIVEN";
		m_FileName = "NO FILE GIVEN";
		auto Folder = "";
		auto image = "";
		general = Beatmap::General();
		meta = Beatmap::Metadata();
		search = Beatmap::SearchBy();
		diff = Beatmap::Difficulty();
		return std::make_unique<Beatmap::Beatmap>(m_FullFilePath, Folder, m_HitsoundsFolder, image, hitobjects, timings, general, meta, search, diff);
	}

	// Cache files if needed
	void Parser::GetAllFiles()
	{
		if (m_ListOfFiles.empty())
		{
			auto count = CacheBeatmaps();
			LOGGER_INFO("Cached {} Beatmaps", count);
			return;
		}
	}

	// Cache files
	int Parser::CacheBeatmaps()
	{
		std::ifstream readFile;
		readFile.open("Beatmaps.prs");
		std::string line;

		// If we cached anything, return the size
		if (!m_ListOfFiles.empty())
		{
			return m_ListOfFiles.size();
		}

		// If we cant find the cache-file, tell the user
		if (!readFile.is_open())
		{
			LOGGER_INFO("Couldn't open => {}", "Beatmaps.prs");
			LOGGER_INFO("Creating now...");
		}
		else // Cache is based on a folder basis. Change the folder and it will re-cache
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

				if (line.length() != 0 && l > 0)
				{
					m_ListOfFiles.push_back(line);
				}

				l++;
			}
			if (l != 0)
			{
				// Return and end the functions, therefore not writing a new cache
				return m_ListOfFiles.size();
			}
		}

		readFile.close();
		
		// Write new Cache
		std::ofstream writeFile;
		writeFile.open("Beatmaps.prs");
		writeFile.write("cached_path_is=", 15);
		writeFile.write(m_SongsFolder.c_str(), strlen(m_SongsFolder.c_str()));
		writeFile.write("\n", 1);

		std::string totalStr("");

		// Because some filenames can be UTF-8, we have to use experimental filesystem. Its also slow, maybe change to BOOST (really big lib tho)
		for (std::experimental::filesystem::recursive_directory_iterator i(m_SongsFolder), end; i != end; ++i)
		{
			// Skip Directories
			if (!is_directory(i->path()))
			{
				LOGGER_TRACE("Element => {}", i->path().string());
				// See: https://stackoverflow.com/a/23658737
				// We find a .osu file
				if (i->path().extension() == ".osu")
				{
					// Filepath formating and adding to variable
					// Doing this to prevent thousands of harddrive writes
					LOGGER_TRACE("Found file => {}", i->path().string());
					std::string RelativeFilePath(i->path().string().erase(0, m_SongsFolder.size()));
					replaceAll(RelativeFilePath, "\\", "/");
					m_ListOfFiles.emplace_back(RelativeFilePath);
					totalStr += RelativeFilePath;
					totalStr += "\n";
				}
			}
		}

		// Write the whole string into file
		writeFile.write(totalStr.c_str(), strlen(totalStr.c_str()));
		writeFile.flush();
		writeFile.close();
		LOGGER_INFO("Cached all beatmaps");
		return m_ListOfFiles.size();
	}

	// Get the Background Image Path from a file
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

	void Parser::ParseHitobjects()
	{
		hitobjects.clear();

		// We only support v13 and v14, older versions have legacy file-formats that are a pain to support
		if (m_TempVersion != "v14" && m_TempVersion != "v13")
		{
			LOGGER_WARN("File Version too old. Cant parse hitobjects.");
			// In case of old file, return empty vector
			return;
		}

		bool hitobjects_start = false;
		size_t counter = 0;

		// Get Amount of Hitobjects to resize vector
		for (auto& line : m_Text)
		{

			if (line.find("[HitObjects]") != std::string::npos) // We found HitObjects
			{
				hitobjects_start = true;
				continue;
			}

			if (hitobjects_start && line.length() >= 16) // The Line has a length thats longer than the minimum possible valid line-length
			{
				counter++;
			}
		}

		hitobjects_start = false;

		// Reserve memory
		hitobjects.reserve(counter);

		// actually fill the thing
		for (auto& line : m_Text)
		{

			if (line.find("[HitObjects]") != std::string::npos) // Basically the same as TimingPoint parsing at this point
			{
				hitobjects_start = true;
				continue;
			}

			if (hitobjects_start && line.length() >= 16)
			{
				LOGGER_TRACE("FOUND HITOBJECT => {}", line);
				LOGGER_TRACE("Parsing Now...");

				std::vector<std::string> parts = split(line, ',');

				auto x = stoi(parts.at(0));
				auto y = stoi(parts.at(1));
				auto offset = stoi(parts.at(2));
				auto type = stoi(parts.at(3));
				auto hitsound = stoi(parts.at(4));
				auto Folder = GetFolder();

				if (stoi(parts.at(3)) & Beatmap::HITCIRCLE)
				{
					// Hitsound calc
					std::vector<std::string> extras = split(parts.at(5), ':');
					std::vector<std::string> hitsoundFileNames;
					std::string set, addition, sampleindex, sound;
					unsigned char vol;
					std::vector<Beatmap::Hitsound> hitsounds;
					Beatmap::TimingPoint redLine, greenLine;

					for (const auto& timingpoint : timings)
					{
						if (timingpoint.GetOffset() <= offset && timingpoint.IsInherited())
						{
							redLine = timingpoint;
						}
						else if (timingpoint.GetOffset() >= offset && timingpoint.IsInherited() && redLine.HasDefaults())
						{
							redLine = timingpoint;
						}
						else if(timingpoint.GetOffset() <= offset)
						{
							greenLine = timingpoint;
						}
					}

					LOGGER_TRACE("Timingpoints for Hitcircle found!");
					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					LOGGER_TRACE("RedLine => {}", redLine);
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
						vol = greenLine.GetVolume();
						LOGGER_TRACE("GreenLine => {}", greenLine);
					}

					// Base hitsound
					if (extras.at(0) == "1")
					{
						set = "normal";
					}
					else if (extras.at(0) == "2")
					{
						set = "soft";
					}
					else if (extras.at(0) == "3")
					{
						set = "drum";
					}

					if (extras.at(2) == "0" && sampleindex != "0")
					{
						sampleindex = sampleindex;
					}
					else if(extras.at(2) != "0" && sampleindex == "0")
					{
						sampleindex = extras.at(2);
					}

					// base Hitsound added
					if (sampleindex == "0")
					{
						// Default sound for this set
						hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hitnormal.wav");
					}
					else
					{
						if (sampleindex == "1")
						{
							sampleindex = "";
						}

						if (!std::filesystem::exists(GetFolder() + set + "-hitnormal" + sampleindex + ".wav"))
						{
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hitnormal.wav");
						}
						else
						{
							// Custom Index
							hitsoundFileNames.emplace_back(GetFolder() + set + "-hitnormal" + sampleindex + ".wav");
						}
					}


					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
					}

					// Addition hitsound
					if (extras.at(1) == "1")
					{
						set = "normal";
					}
					else if (extras.at(1) == "2")
					{
						set = "soft";
					}
					else if (extras.at(1) == "3")
					{
						set = "drum";
					}

					// Whistle check
					if (hitsound & Beatmap::HITSOUND_WHISTLE)
					{
						sound = "whistle";
						if (extras.at(2) != "0" && sampleindex != "0")
						{
							sampleindex = extras.at(2);
						}							
					
						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(GetFolder() + set + "-hit" + sound + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(GetFolder() + set + "-hit" + sound + sampleindex + ".wav");
							}
						}
					}

					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
					}

					// Addition hitsound
					if (extras.at(1) == "1")
					{
						set = "normal";
					}
					else if (extras.at(1) == "2")
					{
						set = "soft";
					}
					else if (extras.at(1) == "3")
					{
						set = "drum";
					}

					// Finish check
					if (hitsound & Beatmap::HITSOUND_FINISH)
					{
						sound = "finish";
						if (extras.at(2) != "0" && sampleindex != "0")
						{
							sampleindex = extras.at(2);
						}

						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(GetFolder() + set + "-hit" + sound + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(GetFolder() + set + "-hit" + sound + sampleindex + ".wav");
							}
						}
					}

					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
					}

					// Addition hitsound
					if (extras.at(1) == "1")
					{
						set = "normal";
					}
					else if (extras.at(1) == "2")
					{
						set = "soft";
					}
					else if (extras.at(1) == "3")
					{
						set = "drum";
					}

					// Clap check
					if (hitsound & Beatmap::HITSOUND_CLAP)
					{
						sound = "clap";
						if (extras.at(2) != "0" && sampleindex != "0")
						{
							sampleindex = extras.at(2);
						}

						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(GetFolder() + set + "-hit" + sound + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(GetFolder() + set + "-hit" + sound + sampleindex + ".wav");
							}
						}
					}


					hitsounds.emplace_back(Beatmap::Hitsound(offset, hitsoundFileNames, vol));

					hitobjects.emplace_back(std::make_shared<Beatmap::Hitcircle>(x, y, offset, type, hitsounds));

				}
				else if (stoi(parts.at(3)) & Beatmap::SLIDER)
				{
					if (parts.size() != 8 && parts.size() != 11)
					{
						LOGGER_WARN("Hitobject has invalid Parts. Expected 11 or 8 Parts (got {})", parts.size());
						LOGGER_INFO("{}", line);
						continue;
					}

					unsigned int repeat = stoi(parts.at(6));
					unsigned int pixelLength = stoi(parts.at(7));
					float durationWithoutBeatLength = pixelLength / (100.0 * diff.GetSliderMultiplier());

					std::vector<std::string> edgeHitsounds;
					std::vector<std::string> edgeAdditions;
					std::vector<std::string> extras;

					if (parts.size() == 11)
					{
						edgeHitsounds = split(parts.at(8), '|'); // Format: 2|0 Meaning SliderHead = 2, SliderEnd = 0 // Always Repeat + 1 Long
						edgeAdditions = split(parts.at(9), '|'); // Format: 0:0|1:0 Meaning SampleSet:Addition|SampleSet2:Addition2 // Always Repeat + 1 long
						extras = split(parts.at(10), ':');
					}
					else
					{
						edgeHitsounds.emplace_back("0"); // Format: 2|0 Meaning SliderHead = 2, SliderEnd = 0 // Always Repeat + 1 Long
						edgeHitsounds.emplace_back("0");
						edgeAdditions.emplace_back("0:0");// Format: 0:0|1:0 Meaning SampleSet:Addition|SampleSet2:Addition2 // Always Repeat + 1 long
						edgeAdditions.emplace_back("0:0");
						extras.emplace_back("0");
						extras.emplace_back("0");
						extras.emplace_back("0");
						extras.emplace_back("0");
					}
					std::vector<Beatmap::Hitsound> hitsounds;
					hitobjects.emplace_back(std::make_shared<Beatmap::Slider>(x, y, offset, type, hitsounds));
				}
				else if (stoi(parts.at(3)) & Beatmap::SPINNER)
				{
					std::vector<std::string> extras = split(parts.at(6), ':');
					std::vector<Beatmap::Hitsound> hitsounds;
					hitobjects.emplace_back(std::make_shared<Beatmap::Spinner>(x, y, offset, type, hitsounds));

				}
				else // Invalid Object
				{
					LOGGER_WARN("Hitobject not supported (most likely 128 = ManiaHoldNote) => {}", line);
				}

			}
		}
	}

	// Parse TimingPoints from a file
	void Parser::ParseTimingPoints()
	{
		// We only support v13 and v14, older versions have legacy file-formats that are a pain to support
		if (m_TempVersion != "v14" && m_TempVersion != "v13")
		{
			LOGGER_WARN("File Version too old. Cant parse timingpoints.");
			// In case of old file, return empty vector
			return;
		}

		bool timingpoints_start = false;
		size_t counter = 0;

		// Get Amount of Timingpoints to resize vector (efficient memory)
		for (auto& line : m_Text)
		{

			if (line.find("[TimingPoints]") != std::string::npos) // We found timingpoints
			{
				timingpoints_start = true;
				continue;
			}

			if (timingpoints_start && line.length() <= 15) // There is 2 Empty lines after the last timingpoint
			{
				timingpoints_start = false;
				break;
			}

			if (timingpoints_start && line.length() >= 15) // Its actually a timingpoint, not just an invalid line
			{
				counter++;
			}
		}

		// Reserve memory
		timings.reserve(counter);

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

			if (timingpoints_start) // If the next lines are timingpoints, parse them
			{
				LOGGER_TRACE("FOUND TIMINGPOINT => {}", line);
				LOGGER_TRACE("Parsing Now...");

				std::vector<std::string> parts = split(line, ',');

				long offset = stol(parts.at(0));
				float milliseconds_per_beat = atof(parts.at(1).c_str());
				unsigned short sampleset = stoi(parts.at(3));
				unsigned short sampleindex = stoi(parts.at(4));
				unsigned short volume = stoi(parts.at(5));
				
				bool inherited = false;

				if (stoi(parts.at(6)) & 1) // If its Inherited (this means you can inherit from it, not that it inherited from somewhere), the first bit is flipped (1 in Base10)
				{
					inherited = true;
				}
				// emplace_back is a modern push_back
				timings.emplace_back(Beatmap::TimingPoint(offset, milliseconds_per_beat, sampleset, sampleindex, volume, inherited));
			}

		}
	}
	// Parse the General Section from a file
	// Logger-messages are self-explanatory
	Beatmap::General Parser::ParseGeneral()
	{
		// Set defaults to compare later on
		std::string FileFormatVersion = "-1";
		std::string AudioFilename = "-1";
		std::string SampleSet = "-1";
		int AudioLeadIn = -1;
		int Mode = -1;

		int found = 0;

		for(auto line : m_Text)
		{
			if (found == 5)
			{
				break;
			}
			
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

			if (line.find("AudioLeadIn: ") != std::string::npos)
			{
				AudioLeadIn = atoi(line.erase(0, 13).c_str());
				LOGGER_TRACE("FOUND AUDIOLEADIN => {}", AudioLeadIn);
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

		return Beatmap::General(FileFormatVersion, AudioFilename, SampleSet, Mode, AudioLeadIn);
	}

	// Parse the Metadata Section from a file
	// Logger-messages are self-explanatory
	Beatmap::Metadata Parser::ParseMetadata()
	{
		// Set defaults to compare later on
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
			{
				break;
			}

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

		return Beatmap::Metadata(Artist, ArtistUnicode, Title, TitleUnicode, Creator, Version);
	}

	// Parse the file for information that can be used for additional searches later on
	// Logger-messages are self-explanatory
	Beatmap::SearchBy Parser::ParseSearchBy()
	{
		// Set defaults to compare later on
		std::string Source = "-1";
		std::string Tags = "-1";
		int         BeatmapID = -1;
		int         BeatmapSetID = -1;

		int found = 0;

		for (auto line : m_Text)
		{

			if (found == 4)
			{
				break;
			}

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

		return Beatmap::SearchBy(Source, Tags, BeatmapID, BeatmapSetID);
	}

	// Parse the Difficulty Section from a file
	// Logger-messages are self-explanatory
	// Note: This is actually rather useless for a music player, but here for completeness
	Beatmap::Difficulty Parser::ParseDifficulty()
	{
		// Set defaults to compare later on
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
			{
				break;
			}

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

		return Beatmap::Difficulty(HPDrainRate, CircleSize, OverallDifficulty, ApproachRate, SliderMultiplier, SliderTickRate);
	}

	std::string Parser::GetFolder()
	{
		auto deleteUntilHere = m_FullFilePath.find_last_of('/', m_FullFilePath.length());
		auto temp = m_FullFilePath;
		return std::string(temp.erase(deleteUntilHere, temp.length()) + "/");
	}

	// Internal function to read a file into a vector to avoid constant harddisk access
	std::vector<std::string> Parser::FileToStringVector(const std::string & filename)
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
}
