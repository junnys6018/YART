project "YART-Lib" 
kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/bin-int/" .. outputdir)

	pchheader "pch.h"
	pchsource "src/pch.cpp"

	files 
	{
		"src/**.h",
		"src/**.cpp",
		"include/**.h"
	}

	links 
	{

	}

	includedirs
	{
		"src",
		"include",
		"vendor/spdlog/include",
	}
	
	filter "configurations:Debug"
		optimize "Debug"
		defines "CONFIGURATION_DEBUG"

	filter "configurations:Release"
		optimize "On"
		defines "CONFIGURATION_RELEASE"

	filter "system:windows"
		defines { "_CRT_SECURE_NO_WARNINGS", "PLATFORM_WINDOWS" }

	filter "system:linux"
		defines { "PLATFORM_LINUX" }
