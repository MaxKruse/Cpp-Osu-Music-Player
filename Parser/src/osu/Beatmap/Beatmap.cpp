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
			: m_FilePath(FilePath), m_HitsoundsFolder(hitsoundFolder), m_Folder(Folder), m_BackgroundImage(BackgroundImage), m_HitObjects(Hitobjects), m_TimingPoints(std::move(Timingpoints)), m_General(std::move(g)), m_Metadata(std::move(m)), m_SearchBy(std::move(s)), m_Difficulty(d), m_Paused(false), m_HandleBase(0), m_HandleFX(0), m_ChannelPos(0)
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
			
			LOGGER_INFO("Destroyed beatmap => {}", GetMetadataText());
		}

		void Beatmap::Load()
		{
			LoadHitsounds();

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
					hs->AddStream(filename, m_HitsoundChannels.at(filename));
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

		void Beatmap::Play()
		{
			LOGGER_INFO("Playing: {}", GetMetadataText());
			if (!m_Paused)
			{
				m_HitsoundsDeleteable = m_Hitsounds;
				Sleep(m_General.GetAudioLeadIn() + 1000);
				BASS_ChannelPlay(m_HandleFX, true);
			}
			else
			{
				BASS_ChannelSetPosition(m_HandleFX, BASS_ChannelGetPosition(m_HandleFX, BASS_POS_BYTE), BASS_POS_BYTE);
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
				float TotalVol = m_SampleVolume / 100.0f * (float)(m_GlobalVolume / 100.0f) * hs->GetVolume() / 100.0;
				if (!m_HitsoundsDeleteable.empty())
				{
					m_HitsoundsDeleteable.front()->ChangePlaybackVolume(TotalVol);
				}
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
			if (criteria.empty())
			{
				return true;
			}

			auto items = split(criteria, ' ');
			return false;
		}

		void Beatmap::LoadHitsounds()
		{
			for (const auto& hitobject : m_HitObjects)
			{
				std::vector<std::string> parts = split(hitobject->GetLine() , ',');
				auto offset = hitobject->GetOffsets().at(0);
				auto hitsound = stoi(parts.at(4));
				auto Folder = m_Folder;
				std::vector<std::string> hitsoundFileNames;
				std::string set, addition, sampleindex, sound;
				std::vector<std::shared_ptr<Hitsound>> hitsounds;
				unsigned char vol;
				TimingPoint redLine, greenLine;

				if(hitobject->GetType() == HITOBJECT_TYPE::HITCIRCLE)
				{ 
					std::vector<std::string> extras = split(parts.at(5), ':');

					for (const auto& timingpoint : m_TimingPoints)
					{
						if (timingpoint.GetOffset() <= offset && timingpoint.IsInherited())
						{
							redLine = timingpoint;
							greenLine = TimingPoint();
						}
						else if (timingpoint.GetOffset() >= offset && timingpoint.IsInherited() && redLine.HasDefaults())
						{
							redLine = timingpoint;
							greenLine = TimingPoint();
						}
						else if (timingpoint.GetOffset() <= offset)
						{
							greenLine = timingpoint;
						}
					}


					LOGGER_TRACE("Timingpoints for Hitcircle found!");
					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					LOGGER_TRACE("RedLine => {}", redLine);
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
						vol = greenLine.GetVolume();
						LOGGER_TRACE("GreenLine => {}", greenLine);
					}

					// Base hitsound
					if (extras.at(0) == "1")
					{
						set = "normal";
					}
					else if (extras.at(0) == "2")
					{
						set = "soft";
					}
					else if (extras.at(0) == "3")
					{
						set = "drum";
					}

					if (extras.at(2) == "0" && sampleindex != "0")
					{
						sampleindex = sampleindex;
					}
					else if (extras.at(2) != "0" && sampleindex == "0")
					{
						sampleindex = extras.at(2);
					}

					// base Hitsound added
					if (sampleindex == "0")
					{
						// Default sound for this set
						hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hitnormal.wav");
					}
					else
					{
						if (sampleindex == "1")
						{
							sampleindex = "";
						}

						if (!std::filesystem::exists(m_Folder + set + "-hitnormal" + sampleindex + ".wav"))
						{
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hitnormal.wav");
						}
						else
						{
							// Custom Index
							hitsoundFileNames.emplace_back(m_Folder + set + "-hitnormal" + sampleindex + ".wav");
						}
					}


					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
						vol = greenLine.GetVolume();
					}

					// Addition hitsound
					if (extras.at(1) == "0")
					{
						if (extras.at(0) == "1")
						{
							set = "normal";
						}
						else if (extras.at(0) == "2")
						{
							set = "soft";
						}
						else if (extras.at(0) == "3")
						{
							set = "drum";
						}
					}
					else if (extras.at(1) == "1")
					{
						set = "normal";
					}
					else if (extras.at(1) == "2")
					{
						set = "soft";
					}
					else if (extras.at(1) == "3")
					{
						set = "drum";
					}

					// Whistle check
					if (hitsound & HITSOUND_WHISTLE)
					{
						sound = "whistle";
						if (extras.at(2) != "0" && sampleindex != "0")
						{
							sampleindex = extras.at(2);
						}

						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(m_Folder + set + "-hit" + sound + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(m_Folder + set + "-hit" + sound + sampleindex + ".wav");
							}
						}
					}

					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
						vol = greenLine.GetVolume();
					}

					// Addition hitsound
					if (extras.at(1) == "0")
					{
						if (extras.at(0) == "1")
						{
							set = "normal";
						}
						else if (extras.at(0) == "2")
						{
							set = "soft";
						}
						else if (extras.at(0) == "3")
						{
							set = "drum";
						}
					}
					else if (extras.at(1) == "1")
					{
						set = "normal";
					}
					else if (extras.at(1) == "2")
					{
						set = "soft";
					}
					else if (extras.at(1) == "3")
					{
						set = "drum";
					}

					// Finish check
					if (hitsound & HITSOUND_FINISH)
					{
						sound = "finish";
						if (extras.at(2) != "0" && sampleindex != "0")
						{
							sampleindex = extras.at(2);
						}

						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(m_Folder + set + "-hit" + sound + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(m_Folder + set + "-hit" + sound + sampleindex + ".wav");
							}
						}
					}

					// Addition hitsound
					if (extras.at(1) == "0")
					{
						if (extras.at(0) == "1")
						{
							set = "normal";
						}
						else if (extras.at(0) == "2")
						{
							set = "soft";
						}
						else if (extras.at(0) == "3")
						{
							set = "drum";
						}
					}
					else if (extras.at(1) == "1")
					{
						set = "normal";
					}
					else if (extras.at(1) == "2")
					{
						set = "soft";
					}
					else if (extras.at(1) == "3")
					{
						set = "drum";
					}

					// Whistle check
					if (hitsound & HITSOUND_CLAP)
					{
						sound = "clap";
						if (extras.at(2) != "0" && sampleindex != "0")
						{
							sampleindex = extras.at(2);
						}

						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(m_Folder + set + "-hit" + sound + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(m_Folder + set + "-hit" + sound + sampleindex + ".wav");
							}
						}
					}

					// Addition hitsound
					if (extras.at(1) == "0")
					{
						if (extras.at(0) == "1")
						{
							set = "normal";
						}
						else if (extras.at(0) == "2")
						{
							set = "soft";
						}
						else if (extras.at(0) == "3")
						{
							set = "drum";
						}
					}
					else if (extras.at(1) == "1")
					{
						set = "normal";
					}
					else if (extras.at(1) == "2")
					{
						set = "soft";
					}
					else if (extras.at(1) == "3")
					{
						set = "drum";
					}

					// Clap check
					if (hitsound & HITSOUND_CLAP)
					{
						sound = "clap";
						if (extras.at(2) != "0" && sampleindex != "0")
						{
							sampleindex = extras.at(2);
						}

						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(m_Folder + set + "-hit" + sound + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(m_Folder + set + "-hit" + sound + sampleindex + ".wav");
							}
						}
					}


					hitsounds.emplace_back(std::make_shared<Hitsound>(offset, hitsoundFileNames, vol));

					hitobject->AddHitsounds(hitsounds);

				}
				else if (hitobject->GetType() == HITOBJECT_TYPE::SLIDER)
				{
					if (parts.size() != 8 && parts.size() != 11)
					{
						LOGGER_WARN("Hitobject has invalid Parts. Expected 11 or 8 Parts (got {})", parts.size());
						LOGGER_INFO("{}", hitobject->GetLine());
						continue;
					}

					for (const auto& timingpoint : m_TimingPoints)
					{
						if (timingpoint.GetOffset() <= offset && timingpoint.IsInherited())
						{
							redLine = timingpoint;
							greenLine = TimingPoint();
						}
						else if (timingpoint.GetOffset() >= offset && timingpoint.IsInherited() && redLine.HasDefaults())
						{
							redLine = timingpoint;
							greenLine = TimingPoint();
						}
						else if (timingpoint.GetOffset() <= offset)
						{
							greenLine = timingpoint;
						}
					}

					LOGGER_TRACE("Timingpoints for Hitcircle found!");

					// Always update and get Timingpoint Data between hitsounds
					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					auto msPerBeat = redLine.GetMillisecondsPerBeat();
					auto sv = 1.0;
					auto repeatOffset = offset;
					LOGGER_TRACE("RedLine => {}", redLine);
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
						vol = greenLine.GetVolume();
						sv = greenLine.GetSvMultiplier();
						LOGGER_TRACE("GreenLine => {}", greenLine);
					}

					size_t repeat = stoi(parts.at(6));
					double pixelLength = stod(parts.at(7));
					double duration = (pixelLength / (100.0 * m_Difficulty.GetSliderMultiplier() * sv) * msPerBeat);

					std::vector<std::string> edgeHitsounds;
					std::vector<std::string> edgeAdditions;
					std::vector<std::string> extras;

					if (parts.size() == 11)
					{
						edgeHitsounds = split(parts.at(8), '|'); // Format: 2|0 Meaning SliderHead = 2, SliderEnd = 0 // Always Repeat + 1 Long
						edgeAdditions = split(parts.at(9), '|'); // Format: 0:0|1:0 Meaning SampleSet:Addition|SampleSet2:Addition2 // Always Repeat + 1 long
					}
					else
					{
						for (size_t i = 0; i < repeat; i++)
						{
							edgeHitsounds.emplace_back("0"); // Format: 2|0 Meaning SliderHead = 2, SliderEnd = 0 // Always Repeat + 1 Long
							edgeHitsounds.emplace_back("0");
							edgeAdditions.emplace_back("0:0");// Format: 0:0|1:0 Meaning SampleSet:Addition|SampleSet2:Addition2 // Always Repeat + 1 long
							edgeAdditions.emplace_back("0:0");
						}
					}

					for (size_t i = 0; i < repeat; i++)
					{
						hitsoundFileNames.clear();
						hitsound = stoi(edgeHitsounds.at(i));
						extras = split(edgeAdditions.at(i), ':');
						repeatOffset = ceil(offset + (i * duration));

						for (const auto& timingpoint : m_TimingPoints)
						{
							if (timingpoint.GetOffset() <= repeatOffset && timingpoint.IsInherited())
							{
								redLine = timingpoint;
								greenLine = TimingPoint();
							}
							else if (timingpoint.GetOffset() >= repeatOffset && timingpoint.IsInherited() && redLine.HasDefaults())
							{
								redLine = timingpoint;
								greenLine = TimingPoint();
							}
							else if (timingpoint.GetOffset() <= repeatOffset)
							{
								greenLine = timingpoint;
							}
						}

						LOGGER_TRACE("Timingpoints for Hitcircle found!");
						set = redLine.GetSampleSet();
						sampleindex = redLine.GetSampleIndex();
						vol = redLine.GetVolume();
						LOGGER_TRACE("RedLine => {}", redLine);
						if (!greenLine.HasDefaults())
						{
							set = greenLine.GetSampleSet();
							sampleindex = greenLine.GetSampleIndex();
							vol = greenLine.GetVolume();
							LOGGER_TRACE("GreenLine => {}", greenLine);
						}

						// Base hitsound
						if (extras.at(0) == "1")
						{
							set = "normal";
						}
						else if (extras.at(0) == "2")
						{
							set = "soft";
						}
						else if (extras.at(0) == "3")
						{
							set = "drum";
						}

						// base Hitsound added
						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hitnormal.wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(m_Folder + set + "-hitnormal" + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hitnormal.wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(m_Folder + set + "-hitnormal" + sampleindex + ".wav");
							}
						}


						set = redLine.GetSampleSet();
						sampleindex = redLine.GetSampleIndex();
						vol = redLine.GetVolume();
						if (!greenLine.HasDefaults())
						{
							set = greenLine.GetSampleSet();
							sampleindex = greenLine.GetSampleIndex();
							vol = greenLine.GetVolume();
						}

						// Addition hitsound
						if (extras.at(1) == "0")
						{
							if (extras.at(0) == "1")
							{
								set = "normal";
							}
							else if (extras.at(0) == "2")
							{
								set = "soft";
							}
							else if (extras.at(0) == "3")
							{
								set = "drum";
							}
						}
						else if (extras.at(1) == "1")
						{
							set = "normal";
						}
						else if (extras.at(1) == "2")
						{
							set = "soft";
						}
						else if (extras.at(1) == "3")
						{
							set = "drum";
						}

						// Whistle check
						if (hitsound & HITSOUND_WHISTLE)
						{
							sound = "whistle";

							if (sampleindex == "0")
							{
								// Default sound for this set
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								if (sampleindex == "1")
								{
									sampleindex = "";
								}

								if (!std::filesystem::exists(m_Folder + set + "-hit" + sound + sampleindex + ".wav"))
								{
									hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
								}
								else
								{
									// Custom Index
									hitsoundFileNames.emplace_back(m_Folder + set + "-hit" + sound + sampleindex + ".wav");
								}
							}
						}

						set = redLine.GetSampleSet();
						sampleindex = redLine.GetSampleIndex();
						vol = redLine.GetVolume();
						if (!greenLine.HasDefaults())
						{
							set = greenLine.GetSampleSet();
							sampleindex = greenLine.GetSampleIndex();
							vol = greenLine.GetVolume();
						}

						// Addition hitsound
						if (extras.at(1) == "0")
						{
							if (extras.at(0) == "1")
							{
								set = "normal";
							}
							else if (extras.at(0) == "2")
							{
								set = "soft";
							}
							else if (extras.at(0) == "3")
							{
								set = "drum";
							}
						}
						else if (extras.at(1) == "1")
						{
							set = "normal";
						}
						else if (extras.at(1) == "2")
						{
							set = "soft";
						}
						else if (extras.at(1) == "3")
						{
							set = "drum";
						}

						// Finish check
						if (hitsound & HITSOUND_FINISH)
						{
							sound = "finish";

							if (sampleindex == "0")
							{
								// Default sound for this set
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								if (sampleindex == "1")
								{
									sampleindex = "";
								}

								if (!std::filesystem::exists(m_Folder + set + "-hit" + sound + sampleindex + ".wav"))
								{
									hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
								}
								else
								{
									// Custom Index
									hitsoundFileNames.emplace_back(m_Folder + set + "-hit" + sound + sampleindex + ".wav");
								}
							}
						}

						set = redLine.GetSampleSet();
						sampleindex = redLine.GetSampleIndex();
						vol = redLine.GetVolume();
						if (!greenLine.HasDefaults())
						{
							set = greenLine.GetSampleSet();
							sampleindex = greenLine.GetSampleIndex();
							vol = greenLine.GetVolume();
						}

						// Addition hitsound
						if (extras.at(1) == "0")
						{
							if (extras.at(0) == "1")
							{
								set = "normal";
							}
							else if (extras.at(0) == "2")
							{
								set = "soft";
							}
							else if (extras.at(0) == "3")
							{
								set = "drum";
							}
						}
						else if (extras.at(1) == "1")
						{
							set = "normal";
						}
						else if (extras.at(1) == "2")
						{
							set = "soft";
						}
						else if (extras.at(1) == "3")
						{
							set = "drum";
						}

						// Clap check
						if (hitsound & HITSOUND_CLAP)
						{
							sound = "clap";

							if (sampleindex == "0")
							{
								// Default sound for this set
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								if (sampleindex == "1")
								{
									sampleindex = "";
								}

								if (!std::filesystem::exists(m_Folder + set + "-hit" + sound + sampleindex + ".wav"))
								{
									hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
								}
								else
								{
									// Custom Index
									hitsoundFileNames.emplace_back(m_Folder + set + "-hit" + sound + sampleindex + ".wav");
								}
							}
						}
						hitsounds.emplace_back(std::make_shared<Hitsound>(repeatOffset, hitsoundFileNames, vol));

						set = redLine.GetSampleSet();
						sampleindex = redLine.GetSampleIndex();
						vol = redLine.GetVolume();
						if (!greenLine.HasDefaults())
						{
							set = greenLine.GetSampleSet();
							sampleindex = greenLine.GetSampleIndex();
							vol = greenLine.GetVolume();
						}

						double sliderTickOffset = msPerBeat * (1.0 / m_Difficulty.GetSliderTickrate());
						double maxAmount = floor(duration / sliderTickOffset);
						repeatOffset = ceil(offset + (i * duration));

						for (double j = 1; j <= maxAmount; j++)
						{
							hitsoundFileNames.clear();
							int tempOffset = round(repeatOffset + (sliderTickOffset * j));
							if (tempOffset == (int)round(offset + duration))
							{
								continue;
							}

							if (sampleindex == "0")
							{
								// Default sound for this set
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-slidertick.wav");
							}
							else
							{
								if (sampleindex == "1")
								{
									sampleindex = "";
								}

								if (!std::filesystem::exists(m_Folder + set + "-slidertick" + sampleindex + ".wav"))
								{
									hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-slidertick.wav");
								}
								else
								{
									// Custom Index
									hitsoundFileNames.emplace_back(m_Folder + set + "-slidertick" + sampleindex + ".wav");
								}
							}

							hitsounds.emplace_back(std::make_shared<Hitsound>(tempOffset, hitsoundFileNames, vol));
						}


					}

					hitsoundFileNames.clear();
					hitsound = stoi(edgeHitsounds.at(repeat));
					extras = split(edgeAdditions.at(repeat), ':');
					repeatOffset = ceil(offset + (repeat * duration));

					for (const auto& timingpoint : m_TimingPoints)
					{
						if (timingpoint.GetOffset() <= repeatOffset && timingpoint.IsInherited())
						{
							redLine = timingpoint;
							greenLine = TimingPoint();
						}
						else if (timingpoint.GetOffset() >= repeatOffset && timingpoint.IsInherited() && redLine.HasDefaults())
						{
							redLine = timingpoint;
							greenLine = TimingPoint();
						}
						else if (timingpoint.GetOffset() <= repeatOffset)
						{
							greenLine = timingpoint;
						}
					}

					LOGGER_TRACE("Timingpoints for Hitcircle found!");
					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					LOGGER_TRACE("RedLine => {}", redLine);
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
						vol = greenLine.GetVolume();
						LOGGER_TRACE("GreenLine => {}", greenLine);
					}

					// Base hitsound
					if (extras.at(0) == "1")
					{
						set = "normal";
					}
					else if (extras.at(0) == "2")
					{
						set = "soft";
					}
					else if (extras.at(0) == "3")
					{
						set = "drum";
					}

					// base Hitsound added
					if (sampleindex == "0")
					{
						// Default sound for this set
						hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hitnormal.wav");
					}
					else
					{
						if (sampleindex == "1")
						{
							sampleindex = "";
						}

						if (!std::filesystem::exists(m_Folder + set + "-hitnormal" + sampleindex + ".wav"))
						{
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hitnormal.wav");
						}
						else
						{
							// Custom Index
							hitsoundFileNames.emplace_back(m_Folder + set + "-hitnormal" + sampleindex + ".wav");
						}
					}


					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
						vol = greenLine.GetVolume();
					}

					// Addition hitsound
					if (extras.at(1) == "0")
					{
						if (extras.at(0) == "1")
						{
							set = "normal";
						}
						else if (extras.at(0) == "2")
						{
							set = "soft";
						}
						else if (extras.at(0) == "3")
						{
							set = "drum";
						}
					}
					else if (extras.at(1) == "1")
					{
						set = "normal";
					}
					else if (extras.at(1) == "2")
					{
						set = "soft";
					}
					else if (extras.at(1) == "3")
					{
						set = "drum";
					}

					// Whistle check
					if (hitsound & HITSOUND_WHISTLE)
					{
						sound = "whistle";

						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(m_Folder + set + "-hit" + sound + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(m_Folder + set + "-hit" + sound + sampleindex + ".wav");
							}
						}
					}

					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
						vol = greenLine.GetVolume();
					}

					// Addition hitsound
					if (extras.at(1) == "0")
					{
						if (extras.at(0) == "1")
						{
							set = "normal";
						}
						else if (extras.at(0) == "2")
						{
							set = "soft";
						}
						else if (extras.at(0) == "3")
						{
							set = "drum";
						}
					}
					else if (extras.at(1) == "1")
					{
						set = "normal";
					}
					else if (extras.at(1) == "2")
					{
						set = "soft";
					}
					else if (extras.at(1) == "3")
					{
						set = "drum";
					}

					// Finish check
					if (hitsound & HITSOUND_FINISH)
					{
						sound = "finish";

						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(m_Folder + set + "-hit" + sound + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(m_Folder + set + "-hit" + sound + sampleindex + ".wav");
							}
						}
					}

					for (const auto& timingpoint : m_TimingPoints)
					{
						if (timingpoint.GetOffset() <= repeatOffset && timingpoint.IsInherited())
						{
							redLine = timingpoint;
							greenLine = TimingPoint();
						}
						else if (timingpoint.GetOffset() >= repeatOffset && timingpoint.IsInherited() && redLine.HasDefaults())
						{
							redLine = timingpoint;
							greenLine = TimingPoint();
						}
						else if (timingpoint.GetOffset() <= repeatOffset)
						{
							greenLine = timingpoint;
						}
					}


					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
						vol = greenLine.GetVolume();
					}

					// Addition hitsound
					if (extras.at(1) == "0")
					{
						if (extras.at(0) == "1")
						{
							set = "normal";
						}
						else if (extras.at(0) == "2")
						{
							set = "soft";
						}
						else if (extras.at(0) == "3")
						{
							set = "drum";
						}
					}
					else if (extras.at(1) == "1")
					{
						set = "normal";
					}
					else if (extras.at(1) == "2")
					{
						set = "soft";
					}
					else if (extras.at(1) == "3")
					{
						set = "drum";
					}

					// Clap check
					if (hitsound & HITSOUND_CLAP)
					{
						sound = "clap";

						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(m_Folder + set + "-hit" + sound + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(m_Folder + set + "-hit" + sound + sampleindex + ".wav");
							}
						}
					}
					hitsounds.emplace_back(std::make_shared<Hitsound>(repeatOffset, hitsoundFileNames, vol));
					hitobject->AddHitsounds(hitsounds);
				}
				else if (hitobject->GetType() == HITOBJECT_TYPE::SPINNER)
				{
					std::vector<std::string> extras = split(parts.at(6), ':');

					for (const auto& timingpoint : m_TimingPoints)
					{
						if (timingpoint.GetOffset() <= offset && timingpoint.IsInherited())
						{
							redLine = timingpoint;
							greenLine = TimingPoint();
						}
						else if (timingpoint.GetOffset() >= offset && timingpoint.IsInherited() && redLine.HasDefaults())
						{
							redLine = timingpoint;
							greenLine = TimingPoint();
						}
						else if (timingpoint.GetOffset() <= offset)
						{
							greenLine = timingpoint;
						}
					}

					LOGGER_TRACE("Timingpoints for Hitcircle found!");
					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					LOGGER_TRACE("RedLine => {}", redLine);
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
						vol = greenLine.GetVolume();
						LOGGER_TRACE("GreenLine => {}", greenLine);
					}

					// Base hitsound
					if (extras.at(0) == "1")
					{
						set = "normal";
					}
					else if (extras.at(0) == "2")
					{
						set = "soft";
					}
					else if (extras.at(0) == "3")
					{
						set = "drum";
					}

					// base Hitsound added
					if (sampleindex == "0")
					{
						// Default sound for this set
						hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hitnormal.wav");
					}
					else
					{
						if (sampleindex == "1")
						{
							sampleindex = "";
						}

						if (!std::filesystem::exists(m_Folder + set + "-hitnormal" + sampleindex + ".wav"))
						{
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hitnormal.wav");
						}
						else
						{
							// Custom Index
							hitsoundFileNames.emplace_back(m_Folder + set + "-hitnormal" + sampleindex + ".wav");
						}
					}


					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
						vol = greenLine.GetVolume();
					}

					// Addition hitsound
					if (extras.at(1) == "0")
					{
						if (extras.at(0) == "1")
						{
							set = "normal";
						}
						else if (extras.at(0) == "2")
						{
							set = "soft";
						}
						else if (extras.at(0) == "3")
						{
							set = "drum";
						}
					}
					else if (extras.at(1) == "1")
					{
						set = "normal";
					}
					else if (extras.at(1) == "2")
					{
						set = "soft";
					}
					else if (extras.at(1) == "3")
					{
						set = "drum";
					}

					// Whistle check
					if (hitsound & HITSOUND_WHISTLE)
					{
						sound = "whistle";
						if (extras.at(2) != "0" && sampleindex != "0")
						{
							sampleindex = extras.at(2);
						}

						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(m_Folder + set + "-hit" + sound + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(m_Folder + set + "-hit" + sound + sampleindex + ".wav");
							}
						}
					}

					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
						vol = greenLine.GetVolume();
					}

					// Addition hitsound
					if (extras.at(1) == "0")
					{
						if (extras.at(0) == "1")
						{
							set = "normal";
						}
						else if (extras.at(0) == "2")
						{
							set = "soft";
						}
						else if (extras.at(0) == "3")
						{
							set = "drum";
						}
					}
					else if (extras.at(1) == "1")
					{
						set = "normal";
					}
					else if (extras.at(1) == "2")
					{
						set = "soft";
					}
					else if (extras.at(1) == "3")
					{
						set = "drum";
					}

					// Finish check
					if (hitsound & HITSOUND_FINISH)
					{
						sound = "finish";
						if (extras.at(2) != "0" && sampleindex != "0")
						{
							sampleindex = extras.at(2);
						}

						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(m_Folder + set + "-hit" + sound + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(m_Folder + set + "-hit" + sound + sampleindex + ".wav");
							}
						}
					}

					set = redLine.GetSampleSet();
					sampleindex = redLine.GetSampleIndex();
					vol = redLine.GetVolume();
					if (!greenLine.HasDefaults())
					{
						set = greenLine.GetSampleSet();
						sampleindex = greenLine.GetSampleIndex();
						vol = greenLine.GetVolume();
					}

					// Addition hitsound
					if (extras.at(1) == "0")
					{
						if (extras.at(0) == "1")
						{
							set = "normal";
						}
						else if (extras.at(0) == "2")
						{
							set = "soft";
						}
						else if (extras.at(0) == "3")
						{
							set = "drum";
						}
					}
					else if (extras.at(1) == "1")
					{
						set = "normal";
					}
					else if (extras.at(1) == "2")
					{
						set = "soft";
					}
					else if (extras.at(1) == "3")
					{
						set = "drum";
					}

					// Clap check
					if (hitsound & HITSOUND_CLAP)
					{
						sound = "clap";
						if (extras.at(2) != "0" && sampleindex != "0")
						{
							sampleindex = extras.at(2);
						}

						if (sampleindex == "0")
						{
							// Default sound for this set
							hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
						}
						else
						{
							if (sampleindex == "1")
							{
								sampleindex = "";
							}

							if (!std::filesystem::exists(m_Folder + set + "-hit" + sound + sampleindex + ".wav"))
							{
								hitsoundFileNames.emplace_back(m_HitsoundsFolder + set + "-hit" + sound + ".wav");
							}
							else
							{
								// Custom Index
								hitsoundFileNames.emplace_back(m_Folder + set + "-hit" + sound + sampleindex + ".wav");
							}
						}
					}
					hitsounds.emplace_back(std::make_shared<Hitsound>(stoi(parts.at(5)), hitsoundFileNames, vol));
					hitobject->AddHitsounds(hitsounds);

				}

			}
		}

	} // namespace Beatmap
} // namespace Parser