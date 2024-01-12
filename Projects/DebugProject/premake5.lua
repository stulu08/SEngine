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
print(BuildDir)
print(InstallDir)

workspace "Assembly"
	startproject "Runtime"
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
	
project "Managed Assembly"
	kind "SharedLib"
	language "C#"
	targetname "ManagedAssembly"
	location "Compiler"
	framework "4.8"
	
	targetdir ("Data/")
	objdir ("Compiler/Temp/Managed")
	
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
		"%{wks.location}/Assets/**.cs"
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
	location "Compiler"
	targetname "NativeAssembly"
	
	targetdir ("Data/")
	objdir ("Compiler/Temp/Native")
	
	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}
	
	files {
		"%{wks.location}/Assets/**.c",
		"%{wks.location}/Assets/**.cpp",
		"%{wks.location}/Assets/**.h",
		"%{wks.location}/Assets/**.inl",
		"%{wks.location}/Assets/**.hpp",
		
		"%{wks.location}/Assets/**.glsl",
		"%{wks.location}/Assets/**.comp"
	}
	
	includedirs {
		"%{wks.location}/Assets",
		"%{wks.location}/Assets/Include",
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
		
project "Runtime"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	location "Compiler"
	targetname ("Runtime-windows-x86_64");
	targetdir ("Data/")
	objdir ("Compiler/Temp/Runtime")

	defines
	{
		"ST_RUNTIME",
		"_CRT_SECURE_NO_WARNINGS"
	}

	files
	{
		"%{wks.location}/Runtime/**.c",
		"%{wks.location}/Runtime/**.cpp",
		"%{wks.location}/Runtime/**.h",
		"%{wks.location}/Runtime/**.inl",
		"%{wks.location}/Runtime/**.hpp",
		
		"%{wks.location}/Runtime/**.glsl",
		"%{wks.location}/Runtime/**.comp"
	}

	includedirs
	{
		"%{wks.location}/Runtime",
		"%{wks.location}/Runtime/Include",
		"%{wks.location}/Assets",
		"%{wks.location}/Assets/Include",
		"%{dependencies}",
		"%{IncludeDir.Stulu}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.mono}"
	}
	links {
		"Native Assembly",
		"Stulu.lib"
		
	}
	
	libdirs {
		BuildDir .. "/Data/Stulu/Native/",
	}
	filter "system:windows"
		systemversion "latest"

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
		kind "WindowedApp"
		runtime "Release"
		optimize "on"
		symbols "off"
