#include "pch.h"

#include "Player.h"


extern "C" {
#define OPPAI_STATIC_HEADER
#include "oppai.c"
}

#include "Core.h"
#include "osu/Parser.h"
#include "cxxtimer.hpp"

#define SI_CONVERT_WIN32
#include "SimpleIni.h"

static void PlayBeatmap(std::vector<std::string>& list, CSimpleIniA& Settings, Parser::Parser& p)
{
	bool foundSongToPlay = false;
	srand(time(NULL));

	do
	{
		foundSongToPlay = false;

		// Get Beatmap
		auto index = Parser::Random(list);
		LOGGER_DEBUG("INDEX TO PLAY: {}", index);
		//auto index = 3898; //FELY SEX
		//auto index = 15411; //RAISE MY SWORD
		//auto index = 9827; // DAN DAN KIKOERU

		// Re-Read values for every beatmap to allow for changes between songs
		auto minStar = Settings.GetDoubleValue("General", "MinStars", 5.0);
		auto cpuSleep = Settings.GetLongValue("General", "CPU_Sleep", 200);
		auto speedup = Settings.GetLongValue("General", "SpeedUp", 0);
		auto hitsoundFolder = std::string(Settings.GetValue("Audio", "HitsoundsLocation", "C:/Program Files(x86)/osu!/DefaultHitsounds/"));

		if (hitsoundFolder.at(hitsoundFolder.length() - 1) != '/' && hitsoundFolder.at(hitsoundFolder.length() - 1) != '\\')
		{
			hitsoundFolder += "/";
		}

		auto criteria = Settings.GetValue("Search", "SearchText", "bpm>=140");
		auto masterVolume = Settings.GetLongValue("Audio", "MasterVolume", 14);
		auto songVolume = Settings.GetLongValue("Audio", "SongVolume", 8);
		auto sampleVolume = Settings.GetLongValue("Audio", "HitsoundVolume", 10);

		auto path = list.at(index);
		auto fullPath = p.GetFolderPath() + path;
		if (!std::filesystem::exists(fullPath))
		{
			LOGGER_ERROR("File doesnt exist => {}", p.GetFolderPath() + path);
			LOGGER_ERROR("Consider resetting/deleting your Beatmaps.prs file");
			continue;
		}

		ezpp_t ez;
		ez = ezpp_new();

		if (ezpp(ez, fullPath.data()))
		{
			LOGGER_ERROR("oppai-ng error: cant load file");
			continue;
		}

		if (ezpp_mode(ez) != MODE_STD)
		{
			LOGGER_WARN("Cant Play (wrong mode) => {}", p.GetFolderPath() + path);
			continue;
		}

		double stars = ezpp_stars(ez);

		if (stars <= minStar)
		{
			LOGGER_WARN("Cant Play (low star rating) => {}", p.GetFolderPath() + path);
			LOGGER_WARN("Needed: {} | Got: {}", minStar, stars);
			continue;
		}


		double bpm;
		QWORD lengthInSeconds;
		QWORD a, b;
		int Offset;
		std::vector<std::vector<long>> offsets;

		auto beatmap = p.BeatmapFromFile(path);

		// Check if beatmap is supported
		if (!beatmap->IsPlayable())
		{
			LOGGER_WARN("Cant Play (Wrong FileVersion) => {}", beatmap->GetMetadataText());
			continue;
		}

		if (!beatmap->Search(criteria))
		{
			LOGGER_WARN("Search Criteria weren't met => {}", criteria);
			continue;
		}


		beatmap->Load();
		foundSongToPlay = true;

		// Debug logs
		LOGGER_TRACE("MP3 for {} => {}", path, beatmap->GetMp3());
		LOGGER_TRACE("Full Path for MP3 => {}", beatmap->GetFullMp3Path());

		// 5. Get Song Length to Display change later
		bpm = beatmap->GetBPM();
		lengthInSeconds = beatmap->GetSongLength();

		// 6. Display Data
		a = (int)floor(lengthInSeconds / 60.0);
		b = (int)floor(fmod(lengthInSeconds, 60));
		LOGGER_INFO("Original Length: {:02d}:{:02d}", a, b);

		LOGGER_ERROR("Playing => {}", beatmap->GetMetadataText());
		LOGGER_DEBUG("{:.2f} stars", stars);

		beatmap->SetGlobalVolume(masterVolume);
		beatmap->SetSongVolume(songVolume);
		beatmap->SetSampleVolume(sampleVolume);
		beatmap->SetSpeedup(speedup);
		beatmap->Play();

		bool prevPause = false;


		while (beatmap->IsPlaying()) { // Bass plays async, While the Channel is playing, sleep to not consume CPU. 
			// Check for the current Position in the channel
			if (Offset = beatmap->GetCurrentOffset())
			{
				beatmap->PlaySamples(Offset);
			}
			std::this_thread::sleep_for(std::chrono::microseconds(cpuSleep));
		}

		ezpp_free(ez);

	} while (!foundSongToPlay);
}


int main(int argc, const char * argv[])
{
	// Init the Logger for the whole Program
	Parser::Logger::Init();

	LOGGER_INFO("Osu! Music Player - Made by [BH]Lithium (osu) / MaxKruse (github)\n");
	
	if (argc > 1)
	{
		LOGGER_DEBUG("Parsing Args now...");
		for (size_t i = 1; i < argc; i++)
		{
			LOGGER_DEBUG("Arg[{0}] => {1}", i, argv[i]);
		}

	}

	LOGGER_INFO("Running Bass => DLL {} | Lib {}", HIWORD(BASS_GetVersion()), BASSVERSION);

	// Init Bass audio device to 48kHz and default sound output
	if (!BASS_Init(-1, 48000, 0, NULL, NULL)) {
		LOGGER_ERROR("Can't initialize Bass device");
		return 0;
	}

	// Settings Manager
	CSimpleIniA* Settings = new CSimpleIniA(false, false, false);
	Settings->LoadFile("settings.ini");
	Settings->SetSpaces(false);

	// File doesnt Exist
	if (!std::filesystem::exists("settings.ini"))
	{
		std::ofstream writeFile;
		writeFile.open("Settings.ini");
		writeFile.close();

		LOGGER_INFO("Couldn't open => {}", "Settings.ini");
		LOGGER_INFO("Creating now...");

		Settings->SetValue("General", "SongsFolder", "C:/Program Files(x86)/osu!/Songs/");
		Settings->SetDoubleValue("General", "MinStars", 5.0);
		Settings->SetLongValue("General", "CPU_Sleep", 200);
		Settings->SetLongValue("General", "SpeedUp", 0);
		Settings->SetValue("Search", "SearchText", "bpm>=140");
		Settings->SetValue("Audio", "HitsoundsLocation", "C:/Program Files(x86)/osu!/DefaultHitsounds/");
		Settings->SetLongValue("Audio", "MasterVolume", 14);
		Settings->SetLongValue("Audio", "SongVolume", 8);
		Settings->SetLongValue("Audio", "HitsoundVolume", 10);
		Settings->SaveFile("settings.ini", true);
		LOGGER_ERROR("Settings.ini created. Make sure you set the Songs Folder before starting this program again.");
		std::cin.get();
		return 1;
	}

	auto folder         = std::string(Settings->GetValue("General", "SongsFolder", "C:/Program Files(x86)/osu!/Songs/"));
	auto hitsoundFolder = std::string(Settings->GetValue("Audio", "HitsoundsLocation", "C:/Program Files(x86)/osu!/DefaultHitsounds/"));
	auto criteria		= std::string(Settings->GetValue("Search", "SearchText", "bpm>=140"));

	if (folder.at(folder.length() - 1) != '/' && folder.at(folder.length() - 1) != '\\')
	{
		folder += "/";
	}

	if (hitsoundFolder.at(hitsoundFolder.length() - 1) != '/' && hitsoundFolder.at(hitsoundFolder.length() - 1) != '\\')
	{
		hitsoundFolder += "/";
	}

	double minStar;
	long cpuSleep;
	long speedup;
	long masterVolume;
	long songVolume;
	long sampleVolume;

	Parser::Parser p(folder, hitsoundFolder);
	auto list = p.GetListOfFiles();

#if _SEARCH
	auto searchTerm = "Dan Dan";
	for (size_t i = 0; i < list.size(); i++)
	{
		if (list.at(i).find(searchTerm) != std::string::npos)
		{
			LOGGER_DEBUG("\"{}\" found at index {}: {}", searchTerm, i, list.at(i));
		}
	}
#endif

	do // Main Loop
	{
		PlayBeatmap(list, *Settings, p);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

	} while (true);

	// On Exit, flush all debug output to logfile
	LOGGER_FLUSH();
	return 0;
}
