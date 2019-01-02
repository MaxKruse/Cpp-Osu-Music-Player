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

	Parser::Parser p("C:\\Dev\\C++ Osu Music Player\\Player\\");
	
	auto Map1 = p.BeatmapFromFile("1.osu");
	
	auto HitsoundsOfTimings = Map1->GetHitsoundsOfTimings();
	
	auto last = Map1->GetLastOffset();

	for (long i = 0; i < last; i++)
	{
		if (!HitsoundsOfTimings[i].empty())
		{
			LOGGER_INFO("Hitsounds to play at OFFSET => {}", i);

			for (auto& sound : HitsoundsOfTimings[i])
			{
				LOGGER_INFO("=> {}", sound);
			}
		}
	}

	delete Map1;
	
	return 0xDEAD;
}