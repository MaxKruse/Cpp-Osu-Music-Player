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
		: m_FilePath(FilePath), m_Folder(Folder), m_BackgroundImage(BackgroundImage), m_HitObjects(std::move(Hitobjects)), m_TimingPoints(std::move(Timingpoints)), m_General(std::move(g)), m_Metadata(std::move(m)), m_SearchBy(std::move(s)), m_Difficulty(d), m_Paused(false)
	{
		LOGGER_INFO("Creating Beatmap From File => {}", FilePath);

		// Pre-parsing all hitsounds for all hitobjects
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
				// Since its not supported, give it a hitsound on its only guaranteed Offset
				m_HitsoundsOnTiming.emplace(object->GetOffsets()[0], temp);
				m_Offsets.emplace_back(object->GetOffsets());
				continue;
			}
			
			m_HitsoundsOnTiming.emplace(object->GetHitsounds(timingpoint_to_use));
			m_Offsets.emplace_back(object->GetOffsets());
		}

		m_HitsoundsOnTimingDeleteable = m_HitsoundsOnTiming;

		// Creating BASS Channels
		if (!(m_BaseChannel = BASS_StreamCreateFile(FALSE, GetFullMp3Path().c_str(), 0, 0, BASS_STREAM_DECODE)) && !(m_BaseChannel = BASS_MusicLoad(FALSE, GetFullMp3Path().c_str(), 0, 0, BASS_MUSIC_RAMP | BASS_MUSIC_PRESCAN, 0)))
		{
			LOGGER_ERROR("Cant create sound, Error {}", BASS_ErrorGetCode());
		}

		if (!(m_FXChannel = BASS_FX_TempoCreate(m_BaseChannel, BASS_FX_FREESOURCE)))
		{
			LOGGER_ERROR("Cant create sound, Error {}", BASS_ErrorGetCode());
		}

		// Set Volume
		BASS_ChannelSetAttribute(m_FXChannel, BASS_ATTRIB_VOL, 0.2f);
	}

	// Manually cleanup the memory we used from Hitobjects. Timingpoints were created as Objects, not pointers, so we dont need to destroy these. That happens by default
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

	void Beatmap::GetMissedHitsounds()
	{
		for (const auto& pair : m_HitsoundsOnTimingDeleteable)
		{
			for (const auto& moreData : pair.second)
			{
				LOGGER_ERROR("Missed sound at {} => {}", pair.first, moreData);
			}
		}
	}

	void Beatmap::Play()
	{
		m_HitsoundsOnTimingDeleteable = m_HitsoundsOnTiming;
		LOGGER_INFO("Playing: {}", GetMetadataText());
		if (!m_Paused)
		{
			Sleep(m_General.GetAudioLeadIn() + 1000);
			BASS_ChannelPlay(m_FXChannel, true);
		}
		else
		{
			BASS_ChannelSetPosition(m_FXChannel, m_ChannelPos, BASS_POS_BYTE);
			BASS_ChannelPlay(m_FXChannel, false);
		}
		m_Paused = false;
	}

	void Beatmap::Pause()
	{
		m_ChannelPos = BASS_ChannelGetPosition(m_FXChannel, BASS_POS_BYTE);
		BASS_ChannelStop(m_FXChannel);
		m_Paused = true;
	}

	void Beatmap::Stop()
	{
		BASS_ChannelStop(m_FXChannel);
		LOGGER_INFO("Stopping FXChannel {}", m_FXChannel);
	}

	void Beatmap::Reset()
	{
		m_ChannelPos = 0;
	}

	void Beatmap::SetVolume(unsigned char Vol)
	{
		BASS_ChannelSetAttribute(m_FXChannel, BASS_ATTRIB_VOL, Vol / 100.0f);
		LOGGER_INFO("Changed volume to {}%", Vol);
	}

	void Beatmap::SetSpeedup(unsigned char Speed)
	{
		BASS_ChannelSetAttribute(m_FXChannel, BASS_ATTRIB_TEMPO, (float)(Speed));
	}

	void Beatmap::PlaySamples(long offset)
	{
		LOGGER_ERROR("PLAYING HITSOUNDS UP TO {}ms", offset);
		do
		{
			for (const auto& pair : m_HitsoundsOnTimingDeleteable)
			{
				LOGGER_INFO("pair.first = [}",pair.first);
				if (offset >= pair.first)
				{
					for (const auto& sound : m_HitsoundsOnTimingDeleteable.at(pair.first))
					{
						// Display each hitsound
						LOGGER_ERROR("Hitsound at {}ms => {}", pair.first, sound);
					}
					m_HitsoundsOnTimingDeleteable.erase(pair.first);
				}
				else
				{
					LOGGER_INFO("All Hitsounds Played");
					return;
				}
				
			}

		} while (m_HitsoundsOnTimingDeleteable.at(offset).size() != 0);
		
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