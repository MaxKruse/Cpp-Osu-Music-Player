#include "pch.h"

#include "Core.h"
#include "osu/Parser.h"
#include "cxxtimer.hpp"

#define OPPAI_IMPLEMENTATION
#include "oppai.c"

#define SI_CONVERT_WIN32
#include "SimpleIni.h"

void PlayBeatmap(const std::string& path, double & minStar, long & cpuSleep, long & speedup, long & masterVolume, long & songVolume, long & sampleVolume, Parser::Parser & p)
{
	double bpm;
	QWORD lengthInSeconds;
	QWORD a, b;
	int Offset;
	std::vector<std::vector<long>> offsets;

	struct parser pstate;
	struct beatmap map;

	struct diff_calc stars;

	FILE* bm;

	p_init(&pstate);
	d_init(&stars);

	// Oppai stuff
	bm = fopen((p.GetFolderPath() + path).c_str(), "r");
	p_map(&pstate, &map, bm);
	fclose(bm);

	if (map.mode != MODE_STD)
	{
		LOGGER_WARN("Mode not supported: {}", map.mode);
		return;
	}
	d_calc(&stars, &map, 0);
	LOGGER_DEBUG("{:2f} stars", stars.total);

	if (stars.total < minStar)
	{
		LOGGER_WARN("Cant Play (low star rating) => {}", p.GetFolderPath() + path);
		return;
	}

	auto beatmap = p.BeatmapFromFile(path);

	// Check if beatmap is supported
	if (!beatmap->IsPlayable())
	{
		LOGGER_WARN("Cant Play (Wrong FileVersion) => {}", beatmap->GetMetadataText());
		return;
	}

	// Debug logs
	LOGGER_DEBUG("MP3 for {} => {}", path, beatmap->GetMp3());
	LOGGER_DEBUG("Full Path for MP3 => {}", beatmap->GetFullMp3Path());

	// 5. Get Song Length to Display change later
	bpm = beatmap->GetBPM();
	lengthInSeconds = beatmap->GetSongLength();

	// 6. Display Data
	a = (int)floor(lengthInSeconds / 60.0);
	b = (int)floor(fmod(lengthInSeconds, 60));
	LOGGER_DEBUG("Original Length: {:02d}:{:02d}", a, b);

	LOGGER_ERROR("Playing => {}", beatmap->GetMetadataText());
	beatmap->SetGlobalVolume(masterVolume);
	beatmap->SetSongVolume(songVolume);
	beatmap->SetSampleVolume(sampleVolume);
	beatmap->SetSpeedup(speedup);
	//beatmap->Play();

	while (beatmap->IsPlaying()) { // Bass plays async, While the Channel is playing, sleep to not consume CPU. 
		// Check for the current Position in the channel
		if (Offset = beatmap->GetCurrentOffset())
		{
			beatmap->PlaySamples(Offset);
		}
		std::this_thread::sleep_for(std::chrono::microseconds(cpuSleep));
	}
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
	Settings->LoadFile("Settings.ini");
	Settings->SetSpaces(false);

	std::ifstream readFile;
	readFile.open("Settings.ini");

	// File doesnt Exist
	if (readFile.is_open())
	{
		readFile.close();
	}
	else
	{
		readFile.close();

		std::ofstream writeFile;
		writeFile.open("Settings.ini");
		writeFile.close();

		LOGGER_INFO("Couldn't open => {}", "Settings.ini");
		LOGGER_INFO("Creating now...");

		Settings->SetValue("General", "SongsFolder", "C:/Program Files(x86)/osu!/Songs/");
		Settings->SetDoubleValue("General", "MinStars", 5.0);
		Settings->SetLongValue("General", "CPU_Sleep", 200);
		Settings->SetLongValue("General", "SpeedUp", 0);
		Settings->SetValue("Audio", "HitsoundsLocation", "C:/Program Files(x86)/osu!/DefaultHitsounds\\");
		Settings->SetLongValue("Audio", "MasterVolume", 14);
		Settings->SetLongValue("Audio", "SongVolume", 8);
		Settings->SetLongValue("Audio", "HitsoundVolume", 10);
		Settings->SaveFile("settings.ini", true);
	}

	auto folder         = Settings->GetValue("General", "SongsFolder", "C:/Program Files(x86)/osu!/Songs/");
	auto minStar        = Settings->GetDoubleValue("General", "MinStars", 5.0);
	auto cpuSleep       = Settings->GetLongValue("General", "CPU_Sleep", 200);
	auto speedup	    = Settings->GetLongValue("General", "SpeedUp", 0);
	auto hitsoundFolder = Settings->GetValue("Audio", "HitsoundsLocation", "C:/Program Files(x86)/osu!/DefaultHitsounds/");
	auto masterVolume   = Settings->GetLongValue("Audio", "MasterVolume", 14);
	auto songVolume     = Settings->GetLongValue("Audio", "SongVolume", 8);
	auto sampleVolume   = Settings->GetLongValue("Audio", "HitsoundVolume", 10);

	Parser::Parser p(folder, hitsoundFolder);
	auto list = p.GetListOfFiles();


	do // Music Playing Loop
	{
		// Get Beatmap
		auto index = Parser::Random(list);

		// Re-Read values for every beatmap to allow for changes between songs
		folder = Settings->GetValue("General", "SongsFolder", "%Appdata%/osu!/Songs/");
		minStar = Settings->GetDoubleValue("General", "MinStars", 5.0);
		cpuSleep = Settings->GetLongValue("General", "CPU_Sleep", 200);
		speedup = Settings->GetLongValue("General", "SpeedUp", 0);
		hitsoundFolder = Settings->GetValue("Audio", "HitsoundsLocation", "C:/Program Files(x86)/osu!/DefaultHitsounds/");
		masterVolume = Settings->GetLongValue("Audio", "MasterVolume", 14);
		songVolume = Settings->GetLongValue("Audio", "SongVolume", 8);
		sampleVolume = Settings->GetLongValue("Audio", "HitsoundVolume", 10);

		// Play beatmap in its own function to **hopefully** fix the memory leak
		PlayBeatmap(list.at(index), minStar, cpuSleep, speedup, masterVolume, songVolume, sampleVolume, p);

	} while (true);

	// On Exit, flush all debug output to logfile
	LOGGER_FLUSH();
	return 1;
}
