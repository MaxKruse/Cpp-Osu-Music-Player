#include "pch.h"
#include "Beatmap.h"

namespace Parser {

	Beatmap::Beatmap()
	{
		LOGGER_WARN("No Beatmap was parsed!");
	}


	/// <summary>
	/// Creates a Basic object for later use
	/// </summary>
	/// <param name="FilePath">The folderpath of the given file, example: "C:\\osu\\Songs\\1 - Suck my Dick\\" with trailing slash </param>
	/// <param name="Hitobjects">A std::vector of all the Hitobjects (Slider, Circle, Spinner)</param>
	/// <param name="g">General Data</param>
	/// <param name="m">Metadata</param>
	/// <param name="s">Data thats used to Search for a specific object later on</param>
	/// <param name="d">Object-specific difficulty settings</param>
	Beatmap::Beatmap(
		const std::string & FilePath, const std::string & BackgroundImage , std::vector<Hitobject> Hitobjects, std::vector<TimingPoint> Timingpoints,
		General& g, Metadata& m, SearchBy& s, Difficulty& d
	)
		: m_FilePath(FilePath), m_BackgroundImage(BackgroundImage) , m_TimingPoints(Timingpoints), m_HitObjects(Hitobjects), m_General(g), m_Metadata(m), m_SearchBy(s), m_Difficulty(d)
	{
		LOGGER_INFO("Creating Beatmap From File => {}", FilePath);
	}

	Beatmap::~Beatmap()
	{
		LOGGER_INFO("Deleting Beatmap From File => {}", m_FilePath);
	}

}