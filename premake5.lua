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

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Stulu/vendor/GLFW/include"
IncludeDir["Glad"] = "Stulu/vendor/Glad/include"
IncludeDir["ImGui"] = "Stulu/vendor/imgui"
IncludeDir["assimp"] = "Stulu/vendor/assimp/include"
IncludeDir["glm"] = "Stulu/vendor/glm"
IncludeDir["stb_image"] = "Stulu/vendor/stb_image"
IncludeDir["noise"] = "Stulu/vendor/ReputelessPerlinNoise"

group "Dependencies"
include "Stulu/vendor/GLFW"
include "Stulu/vendor/Glad"
include "Stulu/vendor/imgui"
include "Stulu/vendor/assimp"
group ""
project "Stulu"
	location "Stulu"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "st_pch.h"
	pchsource "Stulu/src/st_pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/ReputelessPerlinNoise/**.h"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.noise}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"assimp",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"St_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "ST_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ST_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ST_DIST"
		runtime "Release"
		optimize "on"

project "TestProject"
	location "TestProject"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/**.glsl",
		"%{prj.name}/**.sshader"
	}

	includedirs
	{
		"%{prj.name}/src",
		"Stulu/vendor/spdlog/include",
		"Stulu/src",
		"Stulu/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Stulu"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "ST_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ST_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ST_DIST"
		runtime "Release"
		optimize "on"

project "Editor"
	location "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	defines
	{
		"ST_EDITOR"
	}
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/Stulu/**.h",
		"%{prj.name}/Stulu/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"Stulu/vendor/spdlog/include",
		"Stulu/src",
		"Stulu/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Stulu"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "ST_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ST_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ST_DIST"
		runtime "Release"
		optimize "on"