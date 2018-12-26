#pragma once
#include "pch.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "Beatmap.h"

namespace Parser {

	class Parser
	{
	public:
		Beatmap BeatmapFromFile(const std::string& filePath);
		Beatmap BeatmapFromString(const std::string& text);

		inline std::string const GetFilepath() const { return m_File; }
		inline std::string const GetText() const { return m_Text; }
	private:
		std::string m_File;
		std::string m_Text;
	};

}