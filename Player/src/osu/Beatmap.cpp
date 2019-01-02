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
		const std::string FilePath, const std::string BackgroundImage , std::vector<Hitobject*> Hitobjects, std::vector<TimingPoint*> Timingpoints,
		General* g, Metadata* m, SearchBy* s, Difficulty* d
	)
		: m_FilePath(std::move(FilePath)), m_BackgroundImage(std::move(BackgroundImage)), m_HitObjects(std::move(Hitobjects)), m_TimingPoints(std::move(Timingpoints)), m_General(std::move(g)), m_Metadata(std::move(m)), m_SearchBy(std::move(s)), m_Difficulty(std::move(d))
	{
		LOGGER_INFO("Creating Beatmap From File => {}", FilePath);

		m_HitsoundsOnTiming = std::map<long, std::vector<std::string>>();

		for (auto& object : m_HitObjects)
		{
			TimingPoint* timingpoint_to_use = nullptr;
			bool found = false;

			for (auto timingpoint : m_TimingPoints)
			{
				if (found)
				{
					break;
				}

				if (object->GetOffset() < timingpoint->GetOffset() && found == false)
				{
					timingpoint_to_use = timingpoint;
					found = true;
				}
				else if (object->GetOffset() > timingpoint->GetOffset())
				{
					timingpoint_to_use = timingpoint;
					found = true;
				}
			}

			if (!found)
			{
				LOGGER_WARN("Hitobject at {} doesnt seem to have a Timingpoint that affects it.", object->GetOffset());
				LOGGER_WARN("Giving it default hitsounds...");
				std::vector<std::string> temp = std::vector<std::string>();
				temp.push_back("normal-hitnormal.wav");

				m_HitsoundsOnTiming.emplace(std::pair<long, std::vector<std::string>>(object->GetOffset(), temp));
				m_Offsets.emplace_back(object->GetOffset());
				continue;
			}

			m_HitsoundsOnTiming.emplace(std::pair<long, std::vector<std::string>>(object->GetOffset(), object->GetHitsounds(timingpoint_to_use)));
			m_Offsets.emplace_back(object->GetOffset());
		}
	}

	Beatmap::~Beatmap()
	{
		LOGGER_INFO("Deleting Hitobjects From Beatmap => {}", m_FilePath);
		for (auto a : m_HitObjects)
		{
			delete a;
		}

		LOGGER_INFO("Deleting TimingPoints From Beatmap => {}", m_FilePath);
		for (auto a : m_TimingPoints)
		{
			delete a;
		}

		delete m_Difficulty;
		delete m_General;
		delete m_Metadata;
		delete m_SearchBy;

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