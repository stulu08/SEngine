project "Stulu-ScriptCore"
	kind "SharedLib"
	language "C#"
	targetname "Stulu-ScriptCore"
	
	namespace "Stulu"
	framework "4.8"
	
	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")
	files
	{
		"src/**.cs",
	}
	links {
		"Microsoft.Csharp",
		"System",
		"System.Core",
		"System.Data",
		"System.Data.DataSetExtensions",
		"System.Net.Http",
		"System.Xml",
		"System.Xml.Linq",
	}
	filter "configurations:Debug"
		runtime "Debug"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
