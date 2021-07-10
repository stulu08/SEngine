workspace "Stulu"
	architecture "x64"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Stulu"
	location "Stulu"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "st_pch.h"
	pchsource "Stulu/src/st_pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
	
	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"ST_PLATFORM_WINDOWS",
			"ST_DLL_BUILD",
			"_WINDLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/TestProject")
		}
	filter "configurations:Debug"
		defines "ST_DEBUG"
		symbols "On"
		
	filter "configurations:Release"
		defines "ST_RELEASE"
		optimize "On"
		
	filter "configurations:Dist"
		defines "ST_DIST"
		optimize "On"

project "TestProject"
	location "TestProject"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
	
	includedirs
	{
		"Stulu/vendor/spdlog/include",
		"Stulu/src"
	}

	links
	{
		"Stulu"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"ST_PLATFORM_WINDOWS",
			"_WINDLL",
		}

	filter "configurations:Debug"
		defines "ST_DEBUG"
		symbols "On"
		
	filter "configurations:Release"
		defines "ST_RELEASE"
		optimize "On"
		
	filter "configurations:Dist"
		defines "ST_DIST"
		optimize "On"