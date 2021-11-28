workspace "Stulu"
	architecture "x64"
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

staticBuild = true
staticRuntime = true

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

builddir = "%{wks.location}/build/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

vulkanSDK = os.getenv("VULKAN_SDK")
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
IncludeDir["Discord"] = "%{wks.location}/Editor/vendor/discord-rpc/include"

ProjectDir = {}
ProjectDir["Stulu"] = "%{wks.location}/Stulu"
ProjectDir["Discord"] = "%{wks.location}/Editor/vendor/discord-rpc"


group "Dependencies"
include "Stulu/vendor/GLFW"
include "Stulu/vendor/Glad"
include "Stulu/vendor/imgui"
include "Stulu/vendor/assimp"
include "Stulu/vendor/yaml-cpp"
include "Stulu/vendor/ImGuizmo"
include "Editor/vendor/discord-rpc"
group ""

include "Stulu"
include "Editor"
include "VulkanTesting"