#include "pch.h"
#include "Beatmap.h"

namespace Parser {
	namespace Beatmap {

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
			const std::string & FilePath, const std::string & Folder, const std::string & hitsoundFolder, const std::string & BackgroundImage,
			std::vector<std::shared_ptr<Hitobject>> Hitobjects, std::vector<TimingPoint> Timingpoints,
			General g, Metadata m, SearchBy s, Difficulty d
		)
			: m_FilePath(FilePath), m_Folder(Folder), m_BackgroundImage(BackgroundImage), m_HitObjects(Hitobjects), m_TimingPoints(std::move(Timingpoints)), m_General(std::move(g)), m_Metadata(std::move(m)), m_SearchBy(std::move(s)), m_Difficulty(d), m_Paused(false), m_HandleBase(0), m_HandleFX(0), m_ChannelPos(0)
		{
			LOGGER_INFO("Creating Beatmap From File => {}", FilePath);
			
			// creating list of hitsounds to play
			for (const auto& object : m_HitObjects)
			{
				auto Hitsounds = object->GetHitsounds();
				for (const auto& hitsound : Hitsounds)
				{
					m_HitsoundsOnTiming.emplace(hitsound.GetOffset(), hitsound.GetSampleNames());
				}
			}
			m_HitsoundsOnTimingDeleteable = m_HitsoundsOnTiming;

			// Creating BASS Channels
			if (!(m_HandleBase = BASS_StreamCreateFile(FALSE, GetFullMp3Path().c_str(), 0, 0, BASS_STREAM_DECODE)))
			{
				LOGGER_ERROR("Cant create sound, Error {}", BASS_ErrorGetCode());
			}

			if (!(m_HandleFX = BASS_FX_TempoCreate(m_HandleBase, BASS_FX_FREESOURCE)))
			{
				LOGGER_ERROR("Cant create sound, Error {}", BASS_ErrorGetCode());
			}

			// Load Hitsound Samples and add them to the Map
			for (const auto& obj : m_HitObjects)
			{
				for (const auto& hs : obj->GetHitsounds())
				{
					for (const auto& samplename : hs.GetSampleNames())
					{
						// Load Sample
						m_SampleChannels.emplace(std::pair<std::string, QWORD>(samplename, BASS_StreamCreateFile(FALSE, samplename.c_str(), 0,0,0)));

						// Set Volume
						BASS_ChannelSetAttribute(m_SampleChannels[samplename], BASS_ATTRIB_VOL, hs.GetVolume() / 100.0);
					}
				}
			}
			
		}
    
		Beatmap::~Beatmap()
		{
			BASS_StreamFree(m_BaseChannel);
			BASS_StreamFree(m_FXChannel);

			for (const auto& sample : m_SampleChannels)
			{
				BASS_StreamFree(sample.second);
			}
			LOGGER_INFO("Destroyed beatmap => {}", GetMetadataText());
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
				BASS_ChannelPlay(m_HandleFX, true);
			}
			else
			{
				BASS_ChannelSetPosition(m_HandleFX, m_ChannelPos, BASS_POS_BYTE);
				BASS_ChannelPlay(m_HandleFX, false);
			}
			m_Paused = false;
		}

		void Beatmap::Pause()
		{
			m_ChannelPos = BASS_ChannelGetPosition(m_HandleFX, BASS_POS_BYTE);
			BASS_ChannelStop(m_HandleFX);
			m_Paused = true;
		}

		void Beatmap::Stop()
		{
			BASS_ChannelStop(m_HandleFX);
			LOGGER_INFO("Stopping FXChannel {}", m_HandleFX);
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
			float TotalVol = Vol / 100.0f * (float)(m_GlobalVolume / 100.0f);
			BASS_ChannelSetAttribute(m_HandleFX, BASS_ATTRIB_VOL, TotalVol);

			LOGGER_INFO("Changed volume to {}%", Vol);
		}

		void Beatmap::SetSampleVolume(unsigned char Vol)
		{
			float TotalVol = Vol / 100.0f * (float)(m_GlobalVolume / 100.0f);

			for (const auto& Sample : m_SampleChannels)
			{
				BASS_ChannelSetAttribute(Sample.second, BASS_ATTRIB_VOL, TotalVol);
				LOGGER_INFO("Changed volume of sample {} to {}%", Sample.first, TotalVol);
			}
		}

		void Beatmap::SetSpeedup(char Speed)
		{
			BASS_ChannelSetAttribute(m_HandleFX, BASS_ATTRIB_TEMPO, (float)(Speed));
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

	} // namespace Beatmap
} // namespace Parser