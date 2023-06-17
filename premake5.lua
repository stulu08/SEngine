include "./Engine/Stulu/vendor/premake/premake_customization/solution_items.lua"
include "./Engine/Stulu/vendor/premake/premake_customization/generate_doc.lua"
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
			"/ignore:4099"
		}

staticBuild = true
staticRuntime = true

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
builddir = "%{wks.location}/Engine/Build/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

vulkanSDK = os.getenv("VULKAN_SDK")
monoDir = os.getenv("MONO_PATH")
physx = "%{wks.location}/Engine/Stulu/vendor/PhysX"
physxDir = "%{wks.location}/Engine/Stulu/vendor/PhysX"

IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/Engine/Stulu/vendor/spdlog/include"
IncludeDir["GLFW"] = "%{wks.location}/Engine/Stulu/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Engine/Stulu/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Engine/Stulu/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Engine/Stulu/vendor/ImGuizmo"
IncludeDir["assimp"] = "%{wks.location}/Engine/Stulu/vendor/assimp/include"
IncludeDir["glm"] = "%{wks.location}/Engine/Stulu/vendor/glm"
IncludeDir["yaml_cpp"] = "%{wks.location}/Engine/Stulu/vendor/yaml-cpp/include"
IncludeDir["stb_image"] = "%{wks.location}/Engine/Stulu/vendor/stb_image"
IncludeDir["noise"] = "%{wks.location}/Engine/Stulu/vendor/ReputelessPerlinNoise"
IncludeDir["entt"] = "%{wks.location}/Engine/Stulu/vendor/entt"
IncludeDir["Vulkan"] = "%{vulkanSDK}/Include"
IncludeDir["PhysX"] = "%{physx}/include"
IncludeDir["mono"] = "%{monoDir}/include"
IncludeDir["Discord"] = "%{wks.location}/Engine/Editor/vendor/discord-rpc/include"

ProjectDir = {}
ProjectDir["Stulu"] = "%{wks.location}/Engine/Stulu"
ProjectDir["Editor"] = "%{wks.location}/Engine/Editor"
ProjectDir["Runtime"] = "%{wks.location}/Engine/Runtime"
ProjectDir["ScriptCore"] = "%{wks.location}/Engine/Scripting/ScriptCore"
ProjectDir["EditorScriptCore"] = "%{wks.location}/Engine/Scripting/EditorScriptCore"
ProjectDir["Discord"] = "%{wks.location}/Engine/Editor/vendor/discord-rpc"
ProjectDir["DebugProject"] = "%{wks.location}/Projects/DebugProject"

include "Engine/Stulu/vendor/PhysX"
group "Dependencies"
include "Engine/Stulu/vendor/GLFW"
include "Engine/Stulu/vendor/Glad"
include "Engine/Stulu/vendor/imgui"
include "Engine/Stulu/vendor/assimp"
include "Engine/Stulu/vendor/yaml-cpp"
include "Engine/Stulu/vendor/ImGuizmo"
include "Engine/Editor/vendor/discord-rpc"
group "Engine"
include "Engine/Stulu"
include "Engine/Editor"
include "Engine/Runtime"
include "Engine/Scripting"
group ""
