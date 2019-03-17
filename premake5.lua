workspace "MusicPlayer"
	architecture "x64"
	startproject "Player"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}

project "Parser"
	location "Parser"
	kind "SharedLib"
	language "C++"
	characterset "Unicode"
	vectorextensions "AVX"

	staticruntime "Off"

	targetdir ("bin/" .. outputDir .. "/%{prj.name}")
	objdir ("bin/Intemediates/" .. outputDir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "Parser/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/vendor/cxxtimer",
		"Player/vendor/Un4Seen_bass"
	}

	libdirs
	{
		"Player/vendor/Un4Seen_bass"
	}

	links
	{
		"bass.lib",
		"bass_fx.lib"
	}

	defines
	{
		"BUILD_DLL"
	}

	postbuildcommands
	{
		("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputDir .. "/Player/\"")
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

	filter "configurations:Debug"
		defines "_DEBUG"
		optimize "Debug"
		symbols "Full"

	filter "configurations:Release"
		defines "_RELEASE"
		optimize "On"
		symbols "On"

	filter "configurations:Dist"
		defines "_DIST"
		optimize "Size"

include "Player/vendor/SimpleIni"

include "Player/vendor/oppai-ng"

project "Player"
	location "Player"
	kind "ConsoleApp"
	language "C++"
	characterset "Unicode"
	vectorextensions "AVX"

	staticruntime "Off"

	targetdir ("bin/" .. outputDir .. "/%{prj.name}")
	objdir ("bin/Intemediates/" .. outputDir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/oppai-ng/oppai.c"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/Un4Seen_bass",
		"%{prj.name}/vendor/SimpleIni",
		"%{prj.name}/vendor/oppai-ng",
		"Parser/src",
		"Parser/vendor/cxxtimer",
		"Parser/vendor/spdlog/include"
	}

	libdirs
	{
		"%{prj.name}/vendor/Un4Seen_bass"
	}

	links
	{
		"Parser",
		"bass.lib",
		"bass_fx.lib",
		"SimpleIni",
		"oppai-ng"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

	filter "configurations:Debug"
		defines "_DEBUG"
		optimize "Debug"
		symbols "Full"

	filter "configurations:Release"
		defines "_RELEASE"
		optimize "On"
		symbols "On"

	filter "configurations:Dist"
		defines "_DIST"
		optimize "Size"