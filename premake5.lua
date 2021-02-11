workspace "YART"
	architecture "x64"
	configurations {"Debug", "Release"}

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

project "YART" 
kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "src/pch.cpp"

	files 
	{
		"src/**.h",
		"src/**.cpp",
	}

	links 
	{

	}

	includedirs
	{
		"src"
	}
	
	filter "configurations:Debug"
		optimize "Debug"
		defines "CONFIGURATION_DEBUG"

	filter "configurations:Release"
		optimize "On"
		defines "CONFIGURATION_RELEASE"

	filter "system:windows"
		defines { "_CRT_SECURE_NO_WARNINGS", "PLATFORM_WINDOWS" }

	filter {"system:windows", "configurations:Release"}
		postbuildcommands
		{
			"copy \"%{wks.location}bin\\" .. outputdir .. "\\%{prj.name}\\%{prj.name}.exe\" \"%{prj.location}%{prj.name}.exe\""
		}

	filter "system:linux"
		defines { "PLATFORM_LINUX" }

	filter {"system:linux", "configurations:Release"}
		postbuildcommands
		{
			"@cp %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/%{prj.name} %{prj.location}/%{prj.name}.out"
		}