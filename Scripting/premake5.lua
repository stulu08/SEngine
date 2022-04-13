project "ScriptCore"
	location "ScriptCore"
	kind "SharedLib"
	language "C#"
	targetname "Stulu.ScriptCore"
	
	namespace "Stulu"
	framework "4.8"
	
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
	postbuildcommands{
		"{COPYDIR} %{ProjectDir.ScriptCore}/bin/".. outputdir .." %{ProjectDir.Stulu}/bin/" .. outputdir .. "/data",
		"{COPYDIR} %{ProjectDir.ScriptCore}/bin/".. outputdir .. " " .. builddir .. "/data",
		"{COPYDIR} %{ProjectDir.ScriptCore}/bin/".. outputdir .. " %{ProjectDir.DebugProject}/data",
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
	postbuildcommands{
		"{COPYDIR} %{ProjectDir.EditorScriptCore}/bin/".. outputdir .." %{ProjectDir.Stulu}/bin/" .. outputdir .. "/data",
		"{COPYDIR} %{ProjectDir.EditorScriptCore}/bin/".. outputdir .. " " .. builddir .. "/data",
		"{COPYDIR} %{ProjectDir.EditorScriptCore}/bin/".. outputdir .. " %{ProjectDir.DebugProject}/data",
	}
	filter "configurations:Debug"
		runtime "Debug"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"

