include "./Dependencies/premake/premake_customization/solution_items.lua"
include "./Dependencies/premake/premake_customization/generate_doc.lua"
workspace "Stulu"
	startproject "Editor"
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
	solution_items 
	{
		"premake5.lua",
		"Engine/Editor/premake5.lua",
		"Engine/Stulu/premake5.lua",
		"Engine/Runtime/premake5.lua",
		"README.md"
	}
	architecture "x86_64"

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

staticBuild = true

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
builddir = "%{wks.location}/Engine/Build/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Dependencies/Dependencies.lua"

include "Dependencies/PhysX"
group "Dependencies"
include "Dependencies/GLFW"
include "Dependencies/Glad"
include "Dependencies/imgui"
include "Dependencies/assimp"
include "Dependencies/yaml-cpp"
include "Dependencies/ImGuizmo"
include "Dependencies/discord-rpc"
include "Dependencies/premake-core-static-lib/premake5-proj.lua"
group "Engine"
include "Engine/Stulu"
include "Engine/Editor"
include "Engine/Runtime"
include "Engine/Scripting"
include "Engine/Launcher"
group ""