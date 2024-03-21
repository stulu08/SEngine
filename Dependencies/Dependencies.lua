
if InstallDir == nil or InstallDir == '' then
	InstallDir = "%{wks.location}";
end

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
BuildDir = "%{InstallDir}Engine/Build/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
Dependencies = "%{InstallDir}/Dependencies"

ProjectDir = {}
ProjectDir["Stulu"] = "%{InstallDir}/Engine/Stulu"
ProjectDir["Editor"] = "%{InstallDir}/Engine/Editor"
ProjectDir["Launcher"] = "%{InstallDir}/Engine/Launcher"
ProjectDir["ScriptCore"] = "%{InstallDir}/Engine/ScriptCore"
ProjectDir["DebugProject"] = "%{InstallDir}/Projects/DebugProject"
ProjectDir["PhysX"] = "%{Dependencies}/PhysX/"
ProjectDir["mono"] = os.getenv("MONO_PATH")
ProjectDir["Vulkan"] = os.getenv("VULKAN_SDK")
ProjectDir["Discord"] = "%{Dependencies}/discord-game-sdk/"

LibraryDir = {}
LibraryDir["StuluNative"] = "%{BuildDir}/Editor/Data/Stulu/Native/"
LibraryDir["StuluManaged"] = "%{BuildDir}/Editor/Data/Stulu/Managed/"
LibraryDir["mono"] = "%{ProjectDir.mono}/lib"
LibraryDir["Vulkan"] = "%{ProjectDir.Vulkan}/Lib"
LibraryDir["Discord"] = "%{ProjectDir.Discord}/lib/%{cfg.architecture}"

Library = {}
Library["Stulu"] = "Stulu.lib"
Library["OpenGL"] = "opengl32.lib"
Library["Vulkan"] = "vulkan-1.lib"
Library["VulkanLayerUtils"] = "VkLayer_utils.lib"
Library["mono"] = "libmono-static-sgen.lib"
Library["Discord"] = "discord_game_sdk.dll"
Library["DiscordLib"] = "discord_game_sdk.dll-lib"

IncludeDir = {}
IncludeDir["Stulu"] = "%{ProjectDir.Stulu}/src"
IncludeDir["spdlog"] = "%{Dependencies}/spdlog/include"
IncludeDir["GLFW"] = "%{Dependencies}/GLFW/include"
IncludeDir["Glad"] = "%{Dependencies}/Glad/include"
IncludeDir["ImGui"] = "%{Dependencies}/imgui"
IncludeDir["ImGuizmo"] = "%{Dependencies}/ImGuizmo"
IncludeDir["assimp"] = "%{Dependencies}/assimp/include"
IncludeDir["glm"] = "%{Dependencies}/glm"
IncludeDir["yaml_cpp"] = "%{Dependencies}/yaml-cpp/include"
IncludeDir["stb_image"] = "%{Dependencies}/stb_image"
IncludeDir["noise"] = "%{Dependencies}/ReputelessPerlinNoise"
IncludeDir["entt"] = "%{Dependencies}/entt"
IncludeDir["premake"] = "%{Dependencies}/premake-core-static-lib/include"
IncludeDir["Vulkan"] = "%{ProjectDir.Vulkan}/Include"
IncludeDir["PhysX"] = "%{ProjectDir.PhysX}/include"
IncludeDir["mono"] = "%{ProjectDir.mono}/include/mono-2.0"
IncludeDir["Discord"] = "%{ProjectDir.Discord}/cpp"