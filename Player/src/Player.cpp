#include "pch.h"
#include "Core.h"
#include "osu/Parser.h"
#include "cxxtimer.hpp"

#include "bass.h"

static void ShowAllOffsetsOfMap(std::unique_ptr<Parser::Beatmap>& map)
{
	auto hitsoundsOfObjects = map->GetHitsoundsOfTimings();
	auto offsets = map->GetOffsets();
	for (auto& offset : offsets)
	{
		if (hitsoundsOfObjects.find(offset) != hitsoundsOfObjects.end())
		{
			// Hitsound found 
			LOGGER_DEBUG("HITSOUNDS FOR OFFEST {} FOUND =>", offset);
			for (auto& hitsound : hitsoundsOfObjects[offset])
			{
				LOGGER_DEBUG("{}", hitsound);
			}
		}
	}
}

static void Benchmark()
{
	 cxxtimer::Timer timer;

	//Parser::Parser p("C:\\Dev\\C++ Osu Music Player\\Player\\");
	Parser::Parser p("D:/osu/Songs/");
	
	auto ListOfFiles = p.GetListOfFiles();

	MessageBox(nullptr, L"Start Parsing", L"Benchmark", MB_OK);

	std::vector<std::unique_ptr<Parser::Beatmap>> Beatmaps;
	timer.start();
	for (auto& file : ListOfFiles)
	{
		Beatmaps.emplace_back(p.BeatmapFromFile(file));
		LOGGER_DEBUG("Time to parse map => {}ms", timer.count());
		timer.reset();
		timer.start();
	}
	timer.reset();

	MessageBox(nullptr, L"Show Hitsounds", L"Benchmark", MB_OK);

	timer.start();
	for (auto& map : Beatmaps)
	{
		ShowAllOffsetsOfMap(map);	
		LOGGER_DEBUG("Time to parse all hitsounds of map => {}ms", timer.count());
		timer.reset();
		timer.start();
	}
	timer.stop();
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

	LOGGER_INFO("Running Bass => Lib {} | DLL {}", BASS_GetVersion(), BASSVERSION);

	if (!BASS_Init(-1, 44100, 0, NULL, NULL)) {
		LOGGER_ERROR("Can't initialize device");
		return 0;
	}
	
	Benchmark();

	MessageBox(nullptr, L"Benchmark Done", L"Benchmark", MB_OK);

	return 0xDEAD;
}