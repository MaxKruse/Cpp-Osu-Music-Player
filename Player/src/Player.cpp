#include "pch.h"
#include "Core.h"
#include "osu/Parser.h"
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

	// TurboBadger

	//Parser::Parser p("C:/Dev/C++ Osu Music Player/Player/");
	Parser::Parser p("D:/osu/Songs/");
	auto list = p.GetListOfFiles();

	do // Music Playing Loop
	{
		// Get Beatmap and load it
		auto index = Parser::Random(list);
		auto beatmap = p.BeatmapFromFile(list.at(index));

		// Check if beatmap is supported
		if (!beatmap->IsPlayable())
		{
			LOGGER_ERROR("Cant Play => {}", beatmap->GetMetadataText());
			continue;
		}

		// Debug logs
		LOGGER_DEBUG("MP3 for {} => {}", list.at(index), beatmap->GetMp3());
		LOGGER_DEBUG("Full Path for MP3 => {}", beatmap->GetFullMp3Path());

		// 5. Get Song Length to Display change later
		auto bpm = beatmap->GetBPM();
		auto lengthInSeconds = beatmap->GetSongLength();

		// 6. Display Data
		int a = (int)floor(lengthInSeconds / 60);
		int b = (int)floor(fmod(lengthInSeconds, 60));
		LOGGER_DEBUG("Original Length: {:02d}:{:02d}", a, b);
		
		beatmap->SetVolume(4);
		beatmap->SetSpeedup(127);
		beatmap->Play();

		QWORD bytePos = 0;
		int Offset;
		auto offsets = beatmap->GetOffsets();

		while (beatmap->IsPlaying()) { // Bass plays async, While the Channel is playing, sleep to not consume CPU. 
			// Check for the current Position in the channel
			if (Offset = beatmap->GetCurrentOffset())
			{
				beatmap->PlaySamples(Offset);
			}
			std::this_thread::sleep_for(std::chrono::microseconds(200));
			LOGGER_DEBUG("Total Length: {}\t\tOffset: {}", lengthInSeconds, Offset);
		}
		
	}
	while (true); // As long as the user doesnt close the program, we will continue playing forever

	// On Exit, flush all debug output to logfile
	LOGGER_FLUSH();
	return 1;
}
