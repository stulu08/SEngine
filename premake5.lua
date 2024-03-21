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
		"Dependencies/Dependencies.lua",
		"Engine/Stulu/Stulu.lua",
		"Engine/Editor/Editor.lua",
		"Engine/ScripCore/ScripCore.lua",
		"Engine/Launcher/Launcher.lua",
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

include "Dependencies/Dependencies.lua"

include "Dependencies/PhysX"
group "Dependencies"
include "Dependencies/GLFW"
include "Dependencies/Glad"
include "Dependencies/imgui"
include "Dependencies/assimp"
include "Dependencies/yaml-cpp"
include "Dependencies/ImGuizmo"
include "Dependencies/premake-core-static-lib/premake5-proj.lua"
include "Dependencies/discord-game-sdk/CPP SDK.lua"
group "Engine"
include "Engine/Stulu/Stulu.lua"
include "Engine/Editor/Editor.lua"
include "Engine/ScriptCore/ScriptCore.lua"
include "Engine/Launcher/Launcher.lua"
group ""