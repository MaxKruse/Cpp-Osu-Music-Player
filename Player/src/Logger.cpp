#include "pch.h"
#include "Logger.h"

namespace Parser {


	std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;

	void Logger::Init()
	{
		spdlog::set_pattern("%^[%H:%M:%S:%e] %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("DEBUG");
		s_CoreLogger->set_level(spdlog::level::trace);
	}
}