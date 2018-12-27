#include "pch.h"
#include "Beatmap.h"

namespace Parser {

	Beatmap::Beatmap()
	{
		LOGGER_WARN("No Beatmap was parsed!");
	}

	Beatmap::Beatmap(
		const std::string & DirectoryPath, std::vector<Hitobject> Hitobjects,
		General& g, Metadata& m, SearchBy& s, Difficulty& d
	)
		: m_DirectoryPath(DirectoryPath), m_HitObjects(Hitobjects), m_General(g), m_Metadata(m), m_SearchBy(s), m_Difficulty(d)
	{
		LOGGER_INFO("Creating Beatmap \"{}\" with \n{}\n{}\n{}\n{}", DirectoryPath, m_General, m_Metadata, m_SearchBy, m_Difficulty);
	}

	Beatmap::~Beatmap()
	{
		LOGGER_INFO("Deleting Beatmap {}", m_DirectoryPath);
	}

}