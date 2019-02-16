#include "pch.h"

#define OPPAI_IMPLEMENTATION
#include "oppai.c"

#include "Core.h"
#include "osu/Parser.h"

#define SI_CONVERT_WIN32
#include "SimpleIni.h"

#include "cxxtimer.hpp"

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
	CSimpleIniA Settings;
	Settings.LoadFile("settings.ini");
	Settings.SetSpaces(false);

	std::ifstream readFile;
	readFile.open("settings.ini");

	// File doesnt Exist
	if (readFile.is_open())
	{
		readFile.close();
	}
	else
	{
		readFile.close();

		std::ofstream writeFile;
		writeFile.open("settings.ini");
		writeFile.close();

		LOGGER_INFO("Couldn't open => {}", "settings.ini");
		LOGGER_INFO("Creating now...");

		Settings.SetValue("General", "SongsFolder", "C:/Program Files(x86)/osu!/Songs/");
		Settings.SetDoubleValue("General", "MinStars", 5.0);
		Settings.SetLongValue("General", "CPU_Sleep", 200);
		Settings.SetLongValue("Audio", "MasterVolume", 5);
		Settings.SetLongValue("Audio", "SongVolume", 6);
		Settings.SetLongValue("Audio", "HitsoundVolume", 7);
		Settings.SaveFile("settings.ini", true);
	}

	auto folder       = Settings.GetValue("General", "SongsFolder", "%Appdata%/osu!/Songs/");
	auto minStar      = Settings.GetDoubleValue("General", "MinStars", 5.0);
	auto cpuSleep     = Settings.GetLongValue("General", "CPU_Sleep", 200);
	auto masterVolume = Settings.GetLongValue("Audio", "MasterVolume", 5);
	auto songVolume   = Settings.GetLongValue("Audio", "SongVolume", 6);
	auto sampleVolume = Settings.GetLongValue("Audio", "HitsoundVolume", 7);

	Parser::Parser p(folder);
	auto list = p.GetListOfFiles();

	double bpm;
	QWORD lengthInSeconds;
	int a, b;
	QWORD bytePos = 0;
	int Offset;
	std::vector<std::vector<long>> offsets;

	do // Music Playing Loop
	{
		// Get Beatmap and load it
		auto index = Parser::Random(list);
		auto beatmap = p.BeatmapFromFile(list.at(index));

		// Check if beatmap is supported
		if (!beatmap->IsPlayable())
		{
			LOGGER_WARN("Cant Play => {}", beatmap->GetMetadataText());
			continue;
		}

		// Debug logs
		LOGGER_DEBUG("MP3 for {} => {}", list.at(index), beatmap->GetMp3());
		LOGGER_DEBUG("Full Path for MP3 => {}", beatmap->GetFullMp3Path());

		// 5. Get Song Length to Display change later
		bpm = beatmap->GetBPM();
		lengthInSeconds = beatmap->GetSongLength();

		// 6. Display Data
		a = (int)floor(lengthInSeconds / 60);
		b = (int)floor(fmod(lengthInSeconds, 60));
		LOGGER_DEBUG("Original Length: {:02d}:{:02d}", a, b);
		
		LOGGER_ERROR("Playing => {}", beatmap->GetMetadataText());
		beatmap->SetGlobalVolume(masterVolume);
		beatmap->SetSongVolume(songVolume);
		beatmap->SetSampleVolume(sampleVolume);
		offsets = beatmap->GetOffsets();
		beatmap->Play();

		while (beatmap->IsPlaying()) { // Bass plays async, While the Channel is playing, sleep to not consume CPU. 
			// Check for the current Position in the channel
			if (Offset = beatmap->GetCurrentOffset())
			{
				beatmap->PlaySamples(Offset);
			}
			std::this_thread::sleep_for(std::chrono::microseconds(cpuSleep));
		}
		
	}
	while (true); // As long as the user doesnt close the program, we will continue playing forever

	// On Exit, flush all debug output to logfile
	LOGGER_FLUSH();
	return 1;
}
