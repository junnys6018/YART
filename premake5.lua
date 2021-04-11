workspace "YART"
	architecture "x64"
	configurations {"Debug", "Release"}

outputdir = "%{cfg.buildcfg}-%{cfg.system}/%{prj.name}"

include "YART-Lib"
include "YART-Cli"
include "YART-Test"