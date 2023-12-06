workspace "Assembly"
	startproject "EditorProjectAssembly"
	configurations
	{
		"Debug",
		"Release"
	}
	flags
	{
		"MultiProcessorCompile"
	}
	architecture "x64"
	
project "ProjectAssembly"
	kind "SharedLib"
	language "C#"
	targetname "ProjectAssembly"

	framework "4.8"
	
	targetdir ("Data/")
	objdir ("temp/compiler/")
	
	links {
		"Microsoft.Csharp",
		"System",
		"System.Core",
		"System.Data",
		"System.Data.DataSetExtensions",
		"System.Net.Http",
		"System.Xml",
		"System.Xml.Linq",
		"Data/Stulu.ScriptCore.dll",
	}
	files
	{
		"assets/**.cs"
	}
	removefiles { "assets/Editor/**.cs" }
	filter "configurations:Debug"
		runtime "Debug"
		optimize "off"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		
project "EditorProjectAssembly"
	kind "SharedLib"
	language "C#"
	targetname "EditorProjectAssembly"
	
	dependson { "ProjectAssembly" }
	
	framework "4.8"
	
	targetdir ("Data/")
	objdir ("temp/compiler/")
	
	links {
		"Microsoft.Csharp",
		"System",
		"System.Core",
		"System.Data",
		"System.Data.DataSetExtensions",
		"System.Net.Http",
		"System.Xml",
		"System.Xml.Linq",
		
		"Data/Stulu.EditorScriptCore.dll",
		"Data/Stulu.ScriptCore.dll",
	}
	files
	{
		"assets/**.cs",
		"assets/Editor/**.cs"
	}
	filter "configurations:Debug"
		runtime "Debug"
		optimize "off"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"