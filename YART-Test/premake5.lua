project "YART-Test" 
kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/bin-int/" .. outputdir)

	files 
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/catch2/catch_amalgamated.cpp",
		"vendor/catch2/catch_amalgamated.hpp",
	}

	links 
	{
		"YART-Lib",
	}

	includedirs
	{
		"src",
		"vendor/catch2",
		"../YART-Lib/include",
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
		buildoptions 
		{
			"-Wall",
			"-Wextra",
			"-pedantic",
		}
