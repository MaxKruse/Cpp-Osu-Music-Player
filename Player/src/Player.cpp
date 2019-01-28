#include "pch.h"
#include "Core.h"
#include "osu/Parser.h"
#include "cxxtimer.hpp"

#include "bass.h"
#include "bass_fx.h"

static void ShowAllOffsetsOfMap(const std::unique_ptr<Parser::Beatmap>& map)
{
	cxxtimer::Timer timer;
	auto hitsoundsOfObjects = map->GetHitsoundsOfTimings();
	auto offsets = map->GetOffsets();
	for (auto& offsetlist : offsets)
	{
		for (auto& offset : offsetlist)
		{
			timer.start();
			if (hitsoundsOfObjects.find(offset) != hitsoundsOfObjects.end())
			{
				LOGGER_DEBUG("Time to find offset in map => {}ns", timer.count<std::chrono::nanoseconds>());
				timer.reset();
				// Hitsound found 
				LOGGER_DEBUG("HITSOUNDS FOR OFFEST {} FOUND =>", offset);
				for (auto& hitsound : hitsoundsOfObjects[offset])
				{
					LOGGER_DEBUG("{}", hitsound);
				}
			}
		}
	}
}

static void Benchmark()
{
	cxxtimer::Timer timer;

	Parser::Parser p("C:\\Dev\\C++ Osu Music Player\\Player\\");
	//Parser::Parser p("D:/osu/Songs/");
	
	auto ListOfFiles = p.GetListOfFiles();

	MessageBox(nullptr, L"Start Parsing", L"Benchmark", MB_OK);

	std::vector<std::unique_ptr<Parser::Beatmap>> Beatmaps;
	timer.start();
	for (auto& file : ListOfFiles)
	{
		Beatmaps.emplace_back(p.BeatmapFromFile(file));
		LOGGER_DEBUG("Time to parse map => {}ns", timer.count<std::chrono::nanoseconds>());
		timer.reset();
		timer.start();
	}
	timer.reset();

	MessageBox(nullptr, L"Show Hitsounds", L"Benchmark", MB_OK);

	timer.start();
	for (auto& map : Beatmaps)
	{
		ShowAllOffsetsOfMap(map);	
		LOGGER_DEBUG("Time to parse all hitsounds of map => {}ns", timer.count<std::chrono::nanoseconds>());
		timer.reset();
		timer.start();
	}
	timer.stop();

	MessageBox(nullptr, L"Benchmark Done", L"Benchmark", MB_OK);
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
	
	//Benchmark();

	//Parser::Parser p("C:/Dev/C++ Osu Music Player/Player/");
	Parser::Parser p("D:/osu/Songs/");
	auto list = p.GetListOfFiles();

	do // Music Playing Loop
	{
		// Get Beatmap and load it
		auto index = Parser::Random(list);
		auto beatmap = p.BeatmapFromFile(list[index]);

		// Check if beatmap is supported
		if (!beatmap->IsPlayable())
		{
			LOGGER_ERROR("Cant Play => {}", beatmap->GetMetadataText());
			continue;
		}

		// Debug logs
		LOGGER_DEBUG("MP3 for {} => {}", list[index], beatmap->GetMp3());
		LOGGER_DEBUG("Full Path for MP3 => {}", beatmap->GetFullMp3Path());

		// Setup for Bass-Channels
		// Playback works like this:
		// 1. Make Channel Variables
		// 2. Initialize Channel from File
		// 		If using BASS_FX_* to create the actual channel to playback the file, give Bass the BASS_STREAM_DECODE Flag, otherwise just 0
		HSTREAM Channel = 0, ChannelFX = 0;
		if (!(Channel = BASS_StreamCreateFile(FALSE, beatmap->GetFullMp3Path().c_str(), 0, 0, BASS_STREAM_DECODE)) && !(Channel = BASS_MusicLoad(FALSE, beatmap->GetFullMp3Path().c_str(), 0, 0, BASS_MUSIC_RAMP | BASS_MUSIC_PRESCAN, 0)))
		{
			LOGGER_ERROR("Cant create sound, Error {}", BASS_ErrorGetCode());
			return 0xDEAD;
		}
		LOGGER_DEBUG("Channel set => {}", Channel);
		LOGGER_INFO("Playing => {}", beatmap->GetMetadataText());

		// 3. From the Decoded channel, create a TempoChannel which allows to change the tempo
		if (!(ChannelFX = BASS_FX_TempoCreate(Channel, BASS_FX_FREESOURCE)))
		{
			LOGGER_ERROR("Cant create sound, Error {}", BASS_ErrorGetCode());
			return 0xDEAD;
		}

		// 4. Set Volume so we dont get deaf by it playing at full volume
		BASS_ChannelSetAttribute(ChannelFX, BASS_ATTRIB_VOL, 0.03f);

		// 5. Get Song Length to Display change later
		auto bpm = beatmap->GetBPM();
		auto lengthInBytes = BASS_ChannelGetLength(ChannelFX, BASS_POS_BYTE);
		auto lengthInSeconds = BASS_ChannelBytes2Seconds(ChannelFX, lengthInBytes);


		// 6. Display Data
		int a = (int)floor(lengthInSeconds / 60);
		int b = (int)floor(fmod(lengthInSeconds, 60));
		LOGGER_DEBUG("Original Length: {:02d}:{:02d}", a, b);

		// 7. Speed Up
		if (bpm < 180.0)
		{
			// An increase by 33.333[...]% Makes the MP3 Play 1.5 Faster (dont ask me why or how, makes no sense to me. Should be 50% really...)
			auto increase = 100.0f / 3.0f; // Speeding it up apparently messes with the offset detection

			lengthInSeconds = lengthInSeconds * (1.0 - (increase / 100.0));
			LOGGER_DEBUG("Increase was {:.2f}%", increase);

			LOGGER_DEBUG("Changing BPM from {:.2f} to {:.2f}", (float)bpm, (float)bpm * (1.0f + (float)(increase / 100.0f)));
			BASS_ChannelSetAttribute(ChannelFX, BASS_ATTRIB_TEMPO, increase);

			a = (int)floor(lengthInSeconds / 60);
			b = (int)floor(fmod(lengthInSeconds, 60));
			LOGGER_INFO("Modified Length: {:02d}:{:02d}", a, b);
		}
		

		// Get the hitsounds
		auto hitsounds = beatmap->GetHitsoundsOfTimings();

		// Play the Tempo Channel
		BASS_ChannelPlay(ChannelFX, true);

		QWORD bytePos = 0;
		int Offset;
		auto offsets = beatmap->GetOffsets();

		// Wait between songs so we dont go from mp3 to mp3 without giving the listener a break
		Sleep(beatmap->GetAudioleadIn());

		while (BASS_ChannelIsActive(ChannelFX)) { // Bass plays async, While the Channel is playing, sleep to not consume CPU. 
			
			// Check for the current Position in the channel
			if (bytePos = BASS_ChannelGetPosition(ChannelFX, BASS_POS_BYTE))
			{
				Offset = (int)floor(BASS_ChannelBytes2Seconds(ChannelFX, bytePos) * 1000);
				bool found = false;
				int foundOffset = 0;

				for (auto& o : offsets)
				{
					for (auto& u : o)
					{
						found = Offset >= u;

						if (found)
						{
							foundOffset = u;
							for (auto& sound : hitsounds[foundOffset])
							{
								// Display each hitsound
								LOGGER_DEBUG("Hitsound at {}ms => {}", foundOffset, sound);
							}
							hitsounds.erase(foundOffset);
							found = false;
							break;
						}
					}

					if (found)
					{
						break;
					}
				}

				if (!found)
				{
					continue;
				}
			}
		}

		LOGGER_ERROR("Missed Hitsounds: {}", hitsounds.size());

		if (hitsounds.empty())
		{
			continue;
		}

		for (auto& o : offsets)
		{
			for (auto& u : o)
			{
				for (auto& hs : hitsounds[u])
				{
					LOGGER_ERROR("Missed Hitsound at {} => {}", u, hs);
				}
			}
		}
	}
	while (true); // As long as the user doesnt close the program, we will continue playing forever

	// On Exit, flush all debug output to logfile
	LOGGER_FLUSH();
	return 1;
}
