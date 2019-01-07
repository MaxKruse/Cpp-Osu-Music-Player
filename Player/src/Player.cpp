#include "pch.h"
#include "Logger.h"
#include "osu/Parser.h"

cxxtimer::Timer timer;

static void ShowAllOffsetsOfMap(std::unique_ptr<Parser::Beatmap>& map)
{
	auto hitsoundsOfObjects = map->GetHitsoundsOfTimings();
	auto offsets = map->GetOffsets();
	for (auto& offset : offsets)
	{
		if (hitsoundsOfObjects.find(offset) != hitsoundsOfObjects.end())
		{
			// Hitsound found 
			LOGGER_INFO("HITSOUNDS FOR OFFEST {} FOUND =>", offset);
			for (auto& hitsound : hitsoundsOfObjects[offset])
			{
				LOGGER_INFO("{}", hitsound);
			}
		}
	}
}

static void Benchmark()
{
	Parser::Parser p("C:\\Dev\\C++ Osu Music Player\\Player\\");

	MessageBox(NULL, L"Start Parsing", L"Benchmark", MB_OK);

	std::vector<std::unique_ptr<Parser::Beatmap>> Beatmaps;
	timer.start();
	for (size_t i = 0; i < 5; i++)
	{
		Beatmaps.emplace_back(p.BeatmapFromFile("1.osu"));
		LOGGER_INFO("Time to parse map => {}ms", timer.count());
		timer.reset();
		timer.start();
	}
	timer.stop();

	MessageBox(NULL, L"Show Hitsounds", L"Benchmark", MB_OK);

	timer.start();
	for (auto& map : Beatmaps)
	{
		ShowAllOffsetsOfMap(map);	
		LOGGER_INFO("Time to parse all hitsounds of map => {}ms", timer.count());
		timer.reset();
		timer.start();
	}
	timer.stop();
}

int main(int argc, const char * argv[])
{
	// Init the Logger for the whole Program
	Parser::Logger::Init();

	puts("Osu! Music Player - Made by [BH]Lithium (osu) / MaxKruse (github)\n");
	
	if (argc > 1)
	{
		LOGGER_TRACE("Parsing Args now...");
		for (size_t i = 1; i < argc; i++)
		{
			LOGGER_INFO("Arg[{0}] => {1}", i, argv[i]);
		}

	}

	Benchmark();

	MessageBox(NULL, L"Benchmark Done", L"Benchmark", MB_OK);

	return 0xDEAD;
}