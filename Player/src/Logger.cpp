#include "pch.h"
#include "Logger.h"

namespace Parser {

	std::shared_ptr<spdlog::logger> Logger::s_FileLogger;
	std::shared_ptr<spdlog::logger> Logger::s_ConsoleLogger;

	void Logger::Init()
	{
		spdlog::set_pattern("%^[%H:%M:%S:%e] %n: %v%$");

		if (CreateDirectory(L"./logs/", NULL) ||
			ERROR_ALREADY_EXISTS == GetLastError())
		{ }
		else
		{
			throw "Could not create folder \"logs/\"";
		}

		time_t rawtime;
		struct tm * timeinfo;
		char buffer[512];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, sizeof(buffer), ".\\logs\\log-%Y-%m-%d - %H-%M-%S.txt", timeinfo);
		
		const spdlog::filename_t &str(buffer);

		s_FileLogger = spdlog::rotating_logger_mt("FILE", str, 1024 * 1024 * 256, 4);
		s_FileLogger->set_level(spdlog::level::trace);
		spdlog::flush_every(std::chrono::seconds(10));

		s_ConsoleLogger = spdlog::stdout_color_mt("CONSOLE");
#ifdef _DEBUG
		s_ConsoleLogger->set_level(spdlog::level::trace);
#elif _RELEASE
		s_ConsoleLogger->set_level(spdlog::level::info);
#else
		s_ConsoleLogger->set_level(spdlog::level::err);
#endif
	}
}