
if InstallDir == nil or InstallDir == '' then
	InstallDir = "%{wks.location}";
end

vulkanSDK = os.getenv("VULKAN_SDK")
monoDir = os.getenv("MONO_PATH")
dependencies = "%{InstallDir}/Dependencies"
physxDir = "%{dependencies}/PhysX/"

ProjectDir = {}
ProjectDir["Stulu"] = "%{InstallDir}/Engine/Stulu"
ProjectDir["Editor"] = "%{InstallDir}/Engine/Editor"
ProjectDir["Runtime"] = "%{InstallDir}/Engine/Runtime"
ProjectDir["Launcher"] = "%{InstallDir}/Engine/Launcher"
ProjectDir["ScriptCore"] = "%{InstallDir}/Engine/Scripting/ScriptCore"
ProjectDir["EditorScriptCore"] = "%{InstallDir}/Engine/Scripting/EditorScriptCore"
ProjectDir["DebugProject"] = "%{InstallDir}/Projects/DebugProject"

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