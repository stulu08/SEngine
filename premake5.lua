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
			"/ignore:4099"
		}

staticBuild = true
staticRuntime = true

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
builddir = "%{wks.location}/Engine/Build/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

vulkanSDK = os.getenv("VULKAN_SDK")
monoDir = os.getenv("MONO_PATH")
dependencies = "%{wks.location}/Dependencies"
physxDir = "%{dependencies}/PhysX/"

ProjectDir = {}
ProjectDir["Stulu"] = "%{wks.location}/Engine/Stulu"
ProjectDir["Editor"] = "%{wks.location}/Engine/Editor"
ProjectDir["Runtime"] = "%{wks.location}/Engine/Runtime"
ProjectDir["Launcher"] = "%{wks.location}/Engine/Launcher"
ProjectDir["ScriptCore"] = "%{wks.location}/Engine/Scripting/ScriptCore"
ProjectDir["EditorScriptCore"] = "%{wks.location}/Engine/Scripting/EditorScriptCore"
ProjectDir["DebugProject"] = "%{wks.location}/Projects/DebugProject"

IncludeDir = {}
IncludeDir["spdlog"] = "%{dependencies}/spdlog/include"
IncludeDir["GLFW"] = "%{dependencies}/GLFW/include"
IncludeDir["Glad"] = "%{dependencies}/Glad/include"
IncludeDir["ImGui"] = "%{dependencies}/imgui"
IncludeDir["ImGuizmo"] = "%{dependencies}/ImGuizmo"
IncludeDir["assimp"] = "%{dependencies}/assimp/include"
IncludeDir["glm"] = "%{dependencies}/glm"
IncludeDir["yaml_cpp"] = "%{dependencies}/yaml-cpp/include"
IncludeDir["stb_image"] = "%{dependencies}/stb_image"
IncludeDir["noise"] = "%{dependencies}/ReputelessPerlinNoise"
IncludeDir["entt"] = "%{dependencies}/entt"
IncludeDir["Discord"] = "%{dependencies}/discord-rpc/include"
IncludeDir["premake"] = "%{dependencies}/premake-core-static-lib/include"
IncludeDir["Vulkan"] = "%{vulkanSDK}/Include"
IncludeDir["PhysX"] = "%{physxDir}/include"
IncludeDir["mono"] = "%{monoDir}/include/mono-2.0"
IncludeDir["Stulu"] = "%{ProjectDir.Stulu}/src"


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