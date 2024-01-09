newoption {
   trigger     = "InstallDir",
   value       = "path",
   description = "Engine installation directory"
}
newoption {
   trigger     = "BuildDir",
   value       = "path",
   description = "Engine build directory"
}

InstallDir = _OPTIONS["InstallDir"]
BuildDir = _OPTIONS["BuildDir"] 
dofile (InstallDir .. "/Dependencies/Dependencies.lua")

workspace "Assembly"

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
	architecture "x64"
	filter "configurations:Debug"
		defines     "_DEBUG"

	filter "configurations:Release or Dist"
		defines     "NDEBUG"
	
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
	
project "Managed Assembly"
	kind "SharedLib"
	language "C#"
	targetname "ManagedAssembly"

	framework "4.8"
	
	targetdir ("Data/")
	objdir ("Temp/Compiler/Managed")
	
	links {
		"Microsoft.Csharp",
		"System",
		"System.Core",
		"System.Data",
		"System.Data.DataSetExtensions",
		"System.Net.Http",
		"System.Xml",
		"System.Xml.Linq",
		BuildDir .. "/Data/Stulu/Managed/Stulu.ScriptCore.dll",
	}
	files
	{
		"Assets/**.cs"
	}

	filter "configurations:Debug"
		runtime "Debug"
		optimize "off"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
	
	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
		
		
project "Native Assembly"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	
	targetname "NativeAssembly"
	
	targetdir ("Data/")
	objdir ("Temp/Compiler/Native")
	
	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}
	
	files {
		"Assets/**.c",
		"Assets/**.cpp",
		"Assets/**.h",
		"Assets/**.inl",
		"Assets/**.hpp",
		
		"Assets/**.glsl",
		"Assets/**.comp"
	}
	
	includedirs {
		"Assets",
		"%{IncludeDir.Stulu}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.mono}"
	}
	
	links {
		"Stulu.lib"
	}
	
	libdirs {
		BuildDir .. "/Data/Stulu/Native/",
	}

	filter "system:windows"
		systemversion "latest"
		defines "ST_WINDOWS"

	filter "configurations:Debug"
		defines "ST_DEBUG"
		runtime "Debug"
		optimize "off"
		symbols "on"

	filter "configurations:Release"
		defines "ST_RELEASE"
		runtime "Release"
		optimize "on"
		symbols "on"

	filter "configurations:Dist"
		defines "ST_DIST"
		runtime "Release"
		optimize "on"
		symbols "off"
		
		