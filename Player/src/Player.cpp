#include "pch.h"
#include "Logger.h"
#include "osu/Parser.h"

int main(int argc, const char * argv[])
{
	// Init the Logger for the whole Program
	Parser::Logger::Init();

	printf("Osu! Music Player - Made by [BH]Lithium (osu) / MaxKruse (github)\n");
	
	if (argc > 1)
	{
		LOGGER_TRACE("Parsing Args now...");
		for (size_t i = 1; i < argc; i++)
		{
			LOGGER_INFO("Arg[{0}] => {1}", i, argv[i]);
		}

	}

	Sleep(1000);
	Sleep(1000);
	Sleep(1000);
	Sleep(1000);
	Sleep(1000);


	Parser::Parser p("C:\\Dev\\C++ Osu Music Player\\Player\\");

	auto Map1 = p.BeatmapFromFile("1.osu");
	auto Map2 = p.BeatmapFromFile("2.osu");
	auto Map3 = p.BeatmapFromFile("3.osu");
	auto Map4 = p.BeatmapFromFile("4.osu");
	auto Map5 = p.BeatmapFromFile("5.osu");
	auto Map6 = p.BeatmapFromFile("6.osu");
	auto Map7 = p.BeatmapFromFile("7.osu");
		

	Sleep(1000);
	Sleep(1000);
	Sleep(1000);
	Sleep(1000);
	Sleep(1000);

	delete Map1;
	delete Map2;
	delete Map3;
	delete Map4;
	delete Map5;
	delete Map6;
	delete Map7;

	Sleep(1000);
	Sleep(1000);
	Sleep(1000);
	Sleep(1000);
	Sleep(1000);


	return 0xDEAD;
}