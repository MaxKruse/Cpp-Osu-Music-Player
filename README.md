# C++ Osu Music Player

[![Build status](https://ci.appveyor.com/api/projects/status/1g4exu2ywcbq8drn?svg=true)](https://ci.appveyor.com/project/MaxKruse/cpp-osu-music-player)

Meant to be a on-the-side learning experience, this is supposed to be a simple music player, working off of [osu!](https://osu.ppy.sh/) and its files.
I basically want to listen to maps including their hitsounds without actually letting the game run in the background and take up a lot of memory, CPU and GPU.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

You will need the following:

* Bass/Bass_FX DLL (BASS_VERSION 516) and Licenses
* premake5 to build your solution files

### Installing

Installing is simply extracting the `.exe`, as well as the `.dll`s into their own folder. Make sure its somewhere in its own folder, as this program will create a folder `logs` to save all logs in. These Logs can become rather big, so make sure you have enough space.

## Development

This is built and tested with Visual Studio 2017 Enterprise on Windows 10, Build 1809.
This is x64 Only, no support for non-Windows systems.

Also using [Visual Studio Code](https://code.visualstudio.com/) together with [Todo+](https://github.com/fabiospampinato/vscode-todo-plus) for keeping track of my [TODO](TODO).

## Built With

* [CxxTimer](https://github.com/andremaravilha/cxxtimer) - Timer used for benchmarking and debugging
* [spdlog](https://github.com/gabime/spdlog) - Logging purposes
* [Bass](https://un4seen.com) - Audio Processing and playback
* [SimpleIni](https://github.com/brofield/simpleini.git) - Config File Handling

## Contributing

Please read [CONTRIBUTING.md](https://github.com/MaxKruse/Cpp-Osu-Music-Player/graphs/contributors) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

I use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/MaxKruse/Cpp-Osu-Music-Player). 

## Authors

* **Me, Maximilian Kruse** - *Idea, Design + Implementations*

See also the list of [contributors](https://github.com/MaxKruse/Cpp-Osu-Music-Player/contributors) who participated in this project.

## License

This project is licensed under the GNU GENERAL PUBLIC LICENSE  - see the [LICENSE](LICENSE) file for details

## Acknowledgments

* Hat tip to anyone whose code was used
* [ppy](https://github.com/ppy) for creating [osu](https://osu.ppy.sh/)
* Mappers of osu, for providing (sometimes great) maps
