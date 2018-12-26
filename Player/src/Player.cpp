#include "pch.h"
#include "Logger.h"


int main(int argc, const char* argv[])
{
	// Init the Logger for the whole Program
	Parser::Logger::Init();

	printf("Osu! Music Player - Made by [BH]Lithium (osu) / MaxKruse (github)\n");


	if (argc > 1)
	{
		for (size_t i = 1; i < argc; i++)
		{
			LOGGER_INFO("Arg[{0}] => {1}", i, argv[i]);
		}

	}
	system("PAUSE");

	return 0xDEAD;
}