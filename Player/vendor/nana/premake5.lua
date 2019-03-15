project "nana"
    kind "StaticLib"
	language "C++"
	
	characterset "Unicode"
	vectorextensions "AVX"
  
	staticruntime "Off"

	targetdir ("../../../bin/" .. outputDir .. "/%{prj.name}")
	objdir ("../../../bin/Intemediates/" .. outputDir .. "/%{prj.name}")

	files
	{
		"source/**.cpp",
		"source/**.hpp",
		"include/**.cpp",
		"include/**.hpp",
	}
	
	includedirs
	{
		"include"
	}
    
	filter "system:windows"
		systemversion "latest"
		defines
		{
			"WIN32"
		}
		
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