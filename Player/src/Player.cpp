#include "pch.h"
#include "Logger.h"
#include "osu/Parser.h"

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

	Parser::Parser p("C:\\Dev\\C++ Osu Music Player\\Player\\");
	
	{
		for (size_t i = 0; i < 5; i++)
		{
			std::unique_ptr<Parser::Beatmap> Map = p.BeatmapFromFile("1.osu");
		}
	}
	

	Sleep(5000);
	return 0xDEAD;
}