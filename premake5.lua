include "./vendor/premake/premake_customization/solution_items.lua"
include "./vendor/premake/premake_customization/generate_doc.lua"
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
		"Editor/premake5.lua",
		"Stulu/premake5.lua",
		"Runtime/premake5.lua",
		"README.md"
	}
	architecture "x86_64"

	filter "configurations:Debug"
		defines     "_DEBUG"

	filter "configurations:Release or Dist"
		defines     "NDEBUG"

	filter { "system:windows", "configurations:Dist", "toolset:not mingw" }
		flags		{ "LinkTimeOptimization" }

staticBuild = true
staticRuntime = true

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

builddir = "%{wks.location}/build/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


vulkanSDK = os.getenv("VULKAN_SDK")
monoDir = os.getenv("MONO_PATH")
physx = "%{wks.location}/Stulu/vendor/physx"

IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/Stulu/vendor/spdlog/include"
IncludeDir["GLFW"] = "%{wks.location}/Stulu/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Stulu/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Stulu/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Stulu/vendor/ImGuizmo"
IncludeDir["assimp"] = "%{wks.location}/Stulu/vendor/assimp/include"
IncludeDir["glm"] = "%{wks.location}/Stulu/vendor/glm"
IncludeDir["yaml_cpp"] = "%{wks.location}/Stulu/vendor/yaml-cpp/include"
IncludeDir["stb_image"] = "%{wks.location}/Stulu/vendor/stb_image"
IncludeDir["noise"] = "%{wks.location}/Stulu/vendor/ReputelessPerlinNoise"
IncludeDir["entt"] = "%{wks.location}/Stulu/vendor/entt"
IncludeDir["Vulkan"] = "%{vulkanSDK}/Include"
IncludeDir["PhysX"] = "%{physx}/include"
IncludeDir["mono"] = "%{monoDir}/include"
IncludeDir["Discord"] = "%{wks.location}/Editor/vendor/discord-rpc/include"

ProjectDir = {}
ProjectDir["Stulu"] = "%{wks.location}/Stulu"
ProjectDir["Editor"] = "%{wks.location}/Editor"
ProjectDir["Runtime"] = "%{wks.location}/Runtime"
ProjectDir["ScriptCore"] = "%{wks.location}/Scripting/ScriptCore"
ProjectDir["EditorScriptCore"] = "%{wks.location}/Scripting/EditorScriptCore"
ProjectDir["Discord"] = "%{wks.location}/Editor/vendor/discord-rpc"
ProjectDir["DebugProject"] = "%{wks.location}/DebugProject"


group "Dependencies"
include "Stulu/vendor/GLFW"
include "Stulu/vendor/Glad"
include "Stulu/vendor/imgui"
include "Stulu/vendor/assimp"
include "Stulu/vendor/yaml-cpp"
include "Stulu/vendor/ImGuizmo"
include "Editor/vendor/discord-rpc"
group "Misc"
include "Stulu Hub"
group "Engine"
include "Stulu"
include "Editor"
include "Runtime"
group "Scripting"
include "Scripting"
group ""
