#include "pch.h"
#include "Logger.h"

namespace Parser {

	std::shared_ptr<spdlog::logger> Logger::s_FileLogger;
	std::shared_ptr<spdlog::logger> Logger::s_ConsoleLogger;

	void Logger::Init()
	{
		if (m_IsInit)
			return;

		SetConsoleCP(65001);
		spdlog::set_pattern("%^[%H:%M:%S:%e] [%n] %v%$");

		if (CreateDirectory(L"./logs/", NULL) ||
			ERROR_ALREADY_EXISTS == GetLastError())
		{ }
		else
		{
			throw "Could not create folder \"logs/\"";
		}

		// Create the Filename for the logfile
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[64];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, sizeof(buffer), ".\\logs\\log-%Y-%m-%d - %H-%M-%S.log", timeinfo);
		
		const spdlog::filename_t &str(buffer);

		s_FileLogger = spdlog::basic_logger_mt("FILELOG", buffer);
		s_FileLogger->set_level(spdlog::level::debug);
		spdlog::flush_on(spdlog::level::warn);

		// Setup log levels for the console based on compiled version

		s_ConsoleLogger = spdlog::stdout_color_mt("CONSOLE");
#ifdef _DEBUG
		s_ConsoleLogger->set_level(spdlog::level::debug);
#elif _RELEASE
		s_ConsoleLogger->set_level(spdlog::level::warn);
#else
		s_ConsoleLogger->set_level(spdlog::level::err);
#endif
		LOGGER_ERROR("This program will crash when encountering invalid/UTF-8 filenames, such as \"öÈÉlé¥é®éþüAÄÎûéé­éÀéÚé╠éµ\".");
		LOGGER_ERROR("If you cannot see this string, check the Log File => {}", str);
		m_IsInit = true;
	}
}