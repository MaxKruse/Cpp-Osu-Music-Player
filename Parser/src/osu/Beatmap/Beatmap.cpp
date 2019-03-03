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
		}
	
		Beatmap::~Beatmap()
		{
			BASS_StreamFree(m_HandleBase);
			BASS_StreamFree(m_HandleFX);

			for (const auto& pair : m_HitsoundChannels)
			{
				BASS_StreamFree(pair.second);
			}

			for (const auto& hs : m_Hitsounds)
			{
				delete hs;
			}

			for (const auto& hs : m_HitsoundsDeleteable)
			{
				delete hs;
			}
			
			LOGGER_INFO("Destroyed beatmap => {}", GetMetadataText());
		}

		void Beatmap::Load()
		{
			// Load hitsounds
			size_t i = 0;
			for (const auto& object : m_HitObjects)
			{
				for (const auto& hitsound : object->GetHitsounds())
				{
					m_Hitsounds.emplace_back(hitsound);
				}
			}
			m_HitsoundsDeleteable = m_Hitsounds;

			// Hitsound Channels
			for (const auto& hs : m_Hitsounds)
			{
				for (const auto& filename : hs->GetSampleNames())
				{
					if (m_HitsoundChannels.find(filename) == m_HitsoundChannels.end())
					{
						// Not existing, create and store
						m_HitsoundChannels.emplace(std::pair<std::string, HSTREAM>(filename, BASS_StreamCreateFile(FALSE, filename.c_str(), 0, 0, 0)));
					}

					// Regardless of the previous if statement, add the resulting hstream to the hitsound
					hs->AddStream(m_HitsoundChannels.at(filename));
				}
			}

			// Creating BASS Channels
			if (!(m_HandleBase = BASS_StreamCreateFile(FALSE, GetFullMp3Path().c_str(), 0, 0, BASS_STREAM_DECODE)))
			{
				LOGGER_ERROR("Cant create sound, Error {}", BASS_ErrorGetCode());
			}

			if (!(m_HandleFX = BASS_FX_TempoCreate(m_HandleBase, BASS_FX_FREESOURCE)))
			{
				LOGGER_ERROR("Cant create sound, Error {}", BASS_ErrorGetCode());
			}
		}

		void Beatmap::GetMissedHitsounds()
		{
		}

		void Beatmap::Play()
		{
			m_HitsoundsDeleteable = m_Hitsounds;
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
			m_SampleVolume = Vol;
			float TotalVol;

			for (const auto& obj : m_HitObjects)
			{
				for (const auto& hs : obj->GetHitsounds())
				{
					TotalVol = Vol / 100.0f * (float)(m_GlobalVolume / 100.0f) * hs->GetVolume() / 100.0;
					hs->ChangePlaybackVolume(TotalVol);
				}
			}
		}

		void Beatmap::SetSpeedup(char Speed)
		{
			BASS_ChannelSetAttribute(m_HandleFX, BASS_ATTRIB_TEMPO, (float)(Speed));
		}

		void Beatmap::PlaySamples(long offset)
		{
			for (const auto& hs : m_HitsoundsDeleteable)
			{
				if (offset < hs->GetOffset())
				{
					return;
				}

				// Play hitsounds and erase so we dont keep playing the same sound
				hs->Play();
				m_HitsoundsDeleteable.erase(m_HitsoundsDeleteable.begin());

				// Change volume for next sounds already
				float TotalVol = m_SampleVolume / 100.0f * (float)(m_GlobalVolume / 100.0f) * hs->GetVolume() / 100.0;;
				m_HitsoundsDeleteable.front()->ChangePlaybackVolume(TotalVol);
				break;
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

		bool Beatmap::Search(std::string criteria)
		{
			auto items = split(criteria, ' ');
			return false;
		}

	} // namespace Beatmap
} // namespace Parser