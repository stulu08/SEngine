project "Managed Assembly"
	kind "SharedLib"
	language "C#"
	targetname "ManagedAssembly"
	location "%{ObjectDir}"
	framework "4.8"
	
	targetdir ("%{TargetDir}/" .. outputdir .. "/Managed")
	objdir ("%{ObjectDir}/" .. outputdir .. "/Managed")
	
	links {
		"Microsoft.Csharp",
		"System",
		"System.Core",
		"System.Data",
		"System.Data.DataSetExtensions",
		"System.Net.Http",
		"System.Xml",
		"System.Xml.Linq",
		"%{LibraryDir.StuluManaged}/Stulu.ScriptCore.dll",
	}
	files
	{
		"%{AssetDir}/**.cs"
	}

	filter "configurations:Debug"
		runtime "Debug"
		optimize "off"

	filter "configurations:Release"
		runtime "Debug"
		optimize "on"
	
	filter "configurations:Dist"
		runtime "Release"
		optimize "on"