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
	/// <param name="BackfroundImage">The name of the background file. Needs to be concat(full_path_withou_file, backgroundimage) to get the full path</param>
	/// <param name="Hitobjects">A std::vector of all the Hitobjects (Slider, Circle, Spinner)</param>
	/// <param name="g">General Data</param>
	/// <param name="m">Metadata</param>
	/// <param name="s">Data thats used to Search for a specific object later on</param>
	/// <param name="d">Object-specific difficulty settings</param>
	Beatmap::Beatmap(
		const std::string & FilePath, const std::string & Folder, const std::string & BackgroundImage, 
		std::vector<Hitobject*> Hitobjects, std::vector<TimingPoint> Timingpoints, 
		General g, Metadata m, SearchBy s, Difficulty d
	)
		: m_FilePath(FilePath), m_Folder(Folder), m_BackgroundImage(BackgroundImage), m_HitObjects(std::move(Hitobjects)), m_TimingPoints(std::move(Timingpoints)), m_General(std::move(g)), m_Metadata(std::move(m)), m_SearchBy(std::move(s)), m_Difficulty(d)
	{
		LOGGER_INFO("Creating Beatmap From File => {}", FilePath);

		m_HitsoundsOnTiming = std::map<long, std::vector<std::string>>();

		for (auto& object : m_HitObjects)
		{
			TimingPoint timingpoint_to_use = TimingPoint(1,2,3,4,5,true);
			bool found = false;

			for (auto& timingpoint : m_TimingPoints)
			{
				if (found)
				{
					break;
				}

				if (object->GetOffsets()[0] < timingpoint.GetOffset() && found == false)
				{
					timingpoint_to_use = timingpoint;
					found = true;
				}
				else if (object->GetOffsets()[0] > timingpoint.GetOffset())
				{
					timingpoint_to_use = timingpoint;
					found = true;
				}
			}

			if (!found)
			{
				LOGGER_DEBUG("Hitobject at {} doesnt seem to have a Timingpoint that affects it.", object->GetOffsets()[0]);
				LOGGER_DEBUG("Giving it default hitsounds...");
				std::vector<std::string> temp = std::vector<std::string>();
				temp.emplace_back("normal-hitnormal.wav");

				m_HitsoundsOnTiming.emplace(object->GetOffsets()[0], temp);
				m_Offsets.emplace_back(object->GetOffsets());
				continue;
			}
			
			m_HitsoundsOnTiming.emplace(object->GetHitsounds(timingpoint_to_use));
			m_Offsets.emplace_back(object->GetOffsets());
		}
	}

	Beatmap::~Beatmap()
	{
		LOGGER_DEBUG("Deleting Hitobjects From Beatmap => {}", m_FilePath);
		for (auto& object : m_HitObjects)
		{
			delete object;
		}

		m_HitObjects.clear();

		LOGGER_DEBUG("Deleting TimingPoints From Beatmap => {}", m_FilePath);
		m_TimingPoints.clear();
		
	}

	std::string Beatmap::ToString() const
	{
		std::stringstream ss;
		ss << '\n';
		ss << "File => " << m_FilePath << '\n';
		ss << "Background Image Path => " << m_BackgroundImage << '\n';
		ss << m_General << '\n';
		ss << m_Metadata << '\n';
		ss << m_SearchBy << '\n';
		ss << m_Difficulty << '\n';

		return ss.str();
	}

}