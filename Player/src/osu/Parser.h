#pragma once
#include "pch.h"
#include "Beatmap.h"

namespace Parser {

	class Parser
	{
	public:
		Parser(const std::string& SongsFolderPath);

		Beatmap BeatmapFromFile(const std::string& FilePath);
		Beatmap BeatmapFromString(const std::string& Text);

		inline std::string const GetFilepath() const { return m_File; }
		inline std::string const GetText() const { return m_Text; }
	private:
		std::string m_SongsFolder;
		std::string m_File;
		std::string m_Text;
	};

}