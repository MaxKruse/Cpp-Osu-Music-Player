﻿#include "pch.h"
#include "Core.h"
#include "osu/Parser.h"
#include "cxxtimer.hpp"

#include "bass.h"

static void ShowAllOffsetsOfMap(const std::shared_ptr<Parser::Beatmap>& map)
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

	std::vector<std::shared_ptr<Parser::Beatmap>> Beatmaps;
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

	if (!BASS_Init(-1, 44100, BASS_DEVICE_INIT, NULL, NULL)) {
		LOGGER_ERROR("Can't initialize Bass device");
		return 0;
	}
	
	//Benchmark();

	//Parser::Parser p("C:/Dev/C++ Osu Music Player/Player/");
	Parser::Parser p("D:/osu/Songs/");
	auto list = p.GetListOfFiles();
	auto index = Parser::Random(list);
	auto beatmap = p.BeatmapFromFile(list[index]);

	LOGGER_INFO("MP3 for {} => {}", list[index], beatmap->GetMp3());
	LOGGER_INFO("Full Path for MP3 => {}", beatmap->GetFullMp3Path());

	HSTREAM Channel = 0;		
	if (!(Channel = BASS_StreamCreateFile(FALSE, beatmap->GetFullMp3Path().c_str(), 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT)) && !(Channel = BASS_MusicLoad(FALSE, beatmap->GetFullMp3Path().c_str(), 0, 0, BASS_MUSIC_RAMP | BASS_MUSIC_PRESCAN | BASS_MUSIC_DECODE, 0)))
	{
		LOGGER_ERROR("Cant create sound, Error {}", BASS_ErrorGetCode());	
	}
	BASS_ChannelPlay(Channel, TRUE);
	for (size_t i = 0; i < 10000; i++)
	{
		Sleep(1);
	}

	LOGGER_FLUSH();
	return 0xDEAD;
}
