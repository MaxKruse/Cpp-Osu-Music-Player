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

	toolset "msc"

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
		"%{prj.name}/vendor/cxxtimer"
	}

	defines
	{
		"BUILD_DLL"
	}

	postbuildcommands
	{
		"mkdir ../bin/" .. outputDir .. "/Player & {COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputDir .. "/Player"
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



project "Player"
	location "Player"
	kind "ConsoleApp"
	language "C++"
	characterset "Unicode"
	vectorextensions "AVX"

	toolset "msc"

	staticruntime "Off"

	targetdir ("bin/" .. outputDir .. "/%{prj.name}")
	objdir ("bin/Intemediates/" .. outputDir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/Un4Seen_bass",
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
		"bass_fx.lib"
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
