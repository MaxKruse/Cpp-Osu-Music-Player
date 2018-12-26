#pragma once
#include "pch.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/fmt/ostr.h"

namespace Parser {

	class Logger
	{

	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
	};

}

#ifdef _DEBUG

// Client Logging Macros
#define LOGGER_TRACE(...)			::Parser::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define LOGGER_INFO(...)			::Parser::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define LOGGER_WARN(...)			::Parser::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define LOGGER_ERROR(...)			::Parser::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define LOGGER_CRITICAL(...)		::Parser::Logger::GetCoreLogger()->critical(__VA_ARGS__)

#else

#define LOGGER_TRACE(...)	
#define LOGGER_INFO(...)	
#define LOGGER_WARN(...)	
#define LOGGER_ERROR(...)	
#define LOGGER_CRITICAL(...)

#endif