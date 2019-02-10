#include "pch.h"
#include "Beatmap.h"

namespace Parser {
	
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
		: m_FilePath(FilePath), m_Folder(Folder), m_BackgroundImage(BackgroundImage), m_HitObjects(std::move(Hitobjects)), m_TimingPoints(std::move(Timingpoints)), m_General(std::move(g)), m_Metadata(std::move(m)), m_SearchBy(std::move(s)), m_Difficulty(d), m_Paused(false), m_BaseChannel(0), m_FXChannel(0), m_ChannelPos(0)
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

				if (object->GetOffsets().at(0) < timingpoint.GetOffset() && found == false)
				{
					timingpoint_to_use = timingpoint;
					found = true;
				}
				else if (object->GetOffsets().at(0) > timingpoint.GetOffset())
				{
					timingpoint_to_use = timingpoint;
					found = true;
				}
			}

			if (!found)
			{
				LOGGER_DEBUG("Hitobject at {} doesnt seem to have a Timingpoint that affects it.", object->GetOffsets().at(0));
				LOGGER_DEBUG("Giving it default hitsounds...");
				std::vector<std::string> temp = std::vector<std::string>();
				temp.emplace_back("normal-hitnormal.wav");
				// Since its not supported, give it a hitsound on its only guaranteed Offset
				m_HitsoundsOnTiming.emplace(object->GetOffsets().at(0), temp);
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

		// Load Hitsound Samples and add them to the Map
		std::string DefaultHitsoundFolder("C:/Dev/C++ Osu Music Player/default_sounds/");


		// First Priority: Folder Path itself
		for (std::experimental::filesystem::recursive_directory_iterator i(m_Folder), end; i != end; ++i)
		{
			// Skip Directories
			if (!is_directory(i->path()))
			{
				LOGGER_TRACE("Element => {}", i->path().string());
				// See: https://stackoverflow.com/a/23658737
				if (i->path().extension() == ".wav")
				{
					// Filepath formating and writing to file
					LOGGER_DEBUG("Found file => {}", i->path().string());
					std::string SampleLocation = i->path().string();
					std::string Index = i->path().string().erase(0, m_Folder.size());
					m_SampleChannels.emplace(Index, BASS_StreamCreateFile(0, SampleLocation.c_str(), 0, 0, 0));
				}
			}
		}

		// Second Priority: Default Path
		for (std::experimental::filesystem::recursive_directory_iterator i(DefaultHitsoundFolder), end; i != end; ++i)
		{
			// Skip Directories
			if (!is_directory(i->path()))
			{
				LOGGER_TRACE("Element => {}", i->path().string());
				// See: https://stackoverflow.com/a/23658737
				if (i->path().extension() == ".wav")
				{
					// Filepath formating and writing to file
					LOGGER_DEBUG("Found file => {}", i->path().string());
					std::string SampleLocation = i->path().string();
					std::string Index = i->path().string().erase(0, DefaultHitsoundFolder.size());
					if (m_SampleChannels.find(Index) == m_SampleChannels.end()) // cant find this Index, therefore loading
					{
						m_SampleChannels.emplace(Index, BASS_StreamCreateFile(0, SampleLocation.c_str(), 0, 0, 0));
					}
				}
			}
		}

		// Set Volume
		float TotalVol = 0.08f;

		BASS_ChannelSetAttribute(m_FXChannel, BASS_ATTRIB_VOL, TotalVol);

		// Volume Change for all Hitsounds based on base Channel
		float multi = 0.6f;

		for (const auto& Sample : m_SampleChannels)
		{
			BASS_ChannelSetAttribute(Sample.second, BASS_ATTRIB_VOL, TotalVol * multi);
			LOGGER_INFO("Changed volume of sample {} to {}%", Sample.first, TotalVol * multi);
		}
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

	void Beatmap::SetGlobalVolume(unsigned char Vol)
	{
		m_GlobalVolume = Vol;

		LOGGER_INFO("Changed global volume to {}%", Vol);
	}

	void Beatmap::SetSongVolume(unsigned char Vol)
	{
		float TotalVol = Vol / 100.0f * (m_GlobalVolume / 100.0f);
		BASS_ChannelSetAttribute(m_FXChannel, BASS_ATTRIB_VOL, TotalVol);

		LOGGER_INFO("Changed volume to {}%", Vol);
	}

	void Beatmap::SetSampleVolume(unsigned char Vol)
	{
		float TotalVol = Vol / 100.0f * (m_GlobalVolume / 100.0f);

		for (const auto& Sample : m_SampleChannels)
		{
			BASS_ChannelSetAttribute(Sample.second, BASS_ATTRIB_VOL, TotalVol);
			LOGGER_INFO("Changed volume of sample {} to {}%", Sample.first, TotalVol);
		}

		LOGGER_INFO("Changed volume to {}%", Vol);
	}

	void Beatmap::SetSpeedup(char Speed)
	{
		BASS_ChannelSetAttribute(m_FXChannel, BASS_ATTRIB_TEMPO, (float)(Speed));
	}

	void Beatmap::PlaySamples(long offset)
	{
		if (m_HitsoundsOnTimingDeleteable.empty())
		{
			return;
		}

		// Check if offset is smaller than first sound
		for (const auto& pair : m_HitsoundsOnTimingDeleteable)
		{
			if (offset < pair.first) // We dont have any sound before the offset
			{
				return;
			}
			else // We have either a hitsound on the Offset or before it
			{
				LOGGER_DEBUG("PLAYING HITSOUNDS UP TO {}ms", offset);
				break;
			}

		}

		while (true)
		{
			for (const auto& pair : m_HitsoundsOnTimingDeleteable)
			{
				LOGGER_DEBUG("pair.first = {}", pair.first);
				if (offset >= pair.first)
				{
					for (const auto& sound : m_HitsoundsOnTimingDeleteable.at(pair.first))
					{
						// Display each hitsound
						LOGGER_DEBUG("Hitsound at {}ms => {}", pair.first, sound);

						// If the hitsound file doesnt exist, just skip this entire sound
						if (m_SampleChannels.find(sound) == m_SampleChannels.end())
						{
							LOGGER_WARN("Couldn't find .wav file. Skipping Hitsound");
							continue;
						}
						BASS_ChannelPlay(m_SampleChannels.at(sound), true);
					}
					m_HitsoundsOnTimingDeleteable.erase(pair.first);
					return;
				}
				else
				{
					LOGGER_INFO("All Hitsounds Played");
					return;
				}

			}
		}
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