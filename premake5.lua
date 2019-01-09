workspace "C++ Osu Music Player"
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

project "Player"
	location "Player"
	kind "ConsoleApp"
	language "C++"
	characterset "Unicode"

	floatingpoint "Fast"

	targetdir ("bin/" .. outputDir .. "/%{prj.name}")
	objdir ("bin/Intemediates/" .. outputDir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "Player/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/vendor/cxxtimer"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
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