newoption {
   trigger     = "InstallDir",
   value       = "path",
   description = "Engine installation directory"
}

workspace "Assembly"
	startproject "Runtime"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	flags
	{
		"MultiProcessorCompile"
	}
	architecture "x64"
	filter "configurations:Debug"
		defines     "_DEBUG"

	filter "configurations:Release or Dist"
		defines     "NDEBUG"
	
	filter { "system:windows", "configurations:Dist", "toolset:not mingw" }
		flags		{ "LinkTimeOptimization" }
	
	filter "action:vs*"
		linkoptions {
			"/ignore:4006",
			"/ignore:4099",
			"/ignore:4251"
		}
		disablewarnings {
			"4006",
			"4099",
			"4251",
		}
		
EngineSourceDir = _OPTIONS["InstallDir"]
InstallDir = EngineSourceDir
dofile (EngineSourceDir .. "/Dependencies/Dependencies.lua")

TargetDir = "%{wks.location}/Data"
ObjectDir = "%{wks.location}/Compiler"
AssetDir = "%{wks.location}/Assets"
RuntimeDir = "%{wks.location}/Runtime"
RuntimeBuildDir = "%{wks.location}/Build"

group "Dependencies"
dofile (EngineSourceDir .. "/Dependencies/imgui/premake5.lua")
dofile (EngineSourceDir .. "/Dependencies/yaml-cpp/premake5.lua")
group ""

include "Config/Runtime.lua"
include "Config/Native.lua"
include "Config/Managed.lua"