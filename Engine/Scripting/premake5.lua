project "ScriptCore"
	location "ScriptCore"
	kind "SharedLib"
	language "C#"
	targetname "Stulu.ScriptCore"
	namespace "Stulu"
	framework "4.8"
	generate_doc "on"
	targetdir ("ScriptCore/bin/" .. outputdir .. "")
	objdir ("ScriptCore/bin-int/" .. outputdir .. "")
	files
	{
		"ScriptCore/src/**.cs",
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

project "EditorScriptCore"
	location "EditorScriptCore"
	kind "SharedLib"
	language "C#"
	targetname "Stulu.EditorScriptCore"
	namespace "Stulu.Editor"
	framework "4.8"
	generate_doc "on"
	targetdir ("EditorScriptCore/bin/" .. outputdir .. "")
	objdir ("EditorScriptCore/bin-int/" .. outputdir .. "")
	files
	{
		"EditorScriptCore/src/**.cs",
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
		"ScriptCore",
	}
	filter "configurations:Debug"
		runtime "Debug"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"