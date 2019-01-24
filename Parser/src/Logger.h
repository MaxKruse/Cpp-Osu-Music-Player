#pragma once
#include "Core.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include "spdlog/fmt/ostr.h"

namespace Parser {

	class API Logger
	{

	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetFileLogger() { return s_FileLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetConsoleLogger() { return s_ConsoleLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_FileLogger;
		static std::shared_ptr<spdlog::logger> s_ConsoleLogger;

	};

}  // namespace Parser

// Logging Macros
#define LOGGER_TRACE(...)			::Parser::Logger::GetConsoleLogger()->trace(__VA_ARGS__);\
									::Parser::Logger::GetFileLogger()->trace(__VA_ARGS__)

#define LOGGER_DEBUG(...)			::Parser::Logger::GetConsoleLogger()->debug(__VA_ARGS__);\
									::Parser::Logger::GetFileLogger()->debug(__VA_ARGS__)

#define LOGGER_INFO(...)			::Parser::Logger::GetConsoleLogger()->info(__VA_ARGS__);\
									::Parser::Logger::GetFileLogger()->info(__VA_ARGS__)

#define LOGGER_WARN(...)			::Parser::Logger::GetConsoleLogger()->warn(__VA_ARGS__);\
									::Parser::Logger::GetFileLogger()->warn(__VA_ARGS__)

#define LOGGER_ERROR(...)			::Parser::Logger::GetConsoleLogger()->error(__VA_ARGS__);\
									::Parser::Logger::GetFileLogger()->error(__VA_ARGS__)

#define LOGGER_CRITICAL(...)		::Parser::Logger::GetConsoleLogger()->critical(__VA_ARGS__);\
									::Parser::Logger::GetFileLogger()->critical(__VA_ARGS__)

#define LOGGER_FLUSH()				::Parser::Logger::GetConsoleLogger()->flush();\
									::Parser::Logger::GetFileLogger()->flush()