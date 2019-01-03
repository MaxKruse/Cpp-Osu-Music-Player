#include "pch.h"
#include "Logger.h"
#include "osu/Parser.h"

static void Benchmark()
{
	Parser::Parser p("C:\\Dev\\C++ Osu Music Player\\Player\\");

	std::unique_ptr<Parser::Beatmap> Map1 = p.BeatmapFromFile("1.osu");
	std::unique_ptr<Parser::Beatmap> Map2 = p.BeatmapFromFile("1.osu");
	std::unique_ptr<Parser::Beatmap> Map3 = p.BeatmapFromFile("1.osu");
	std::unique_ptr<Parser::Beatmap> Map4 = p.BeatmapFromFile("1.osu");
	std::unique_ptr<Parser::Beatmap> Map5 = p.BeatmapFromFile("1.osu");
	std::unique_ptr<Parser::Beatmap> Map6 = p.BeatmapFromFile("1.osu");
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

	Sleep(5000);
	return 0xDEAD;
}