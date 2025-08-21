project "ScriptCore"
	kind "SharedLib"
	language "C#"
	targetname "Stulu.ScriptCore"
	namespace "Stulu"
	framework "4.8"
	generate_doc "on"
	clr "Unsafe"
	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")
	files
	{
		"src/**.cs",
	}
	
	-- get all links from Packages folder, since using wildcards here breaks premake...
	local dll_pattern = "Packages/*.dll"
	local fullpath_dlls = os.matchfiles(dll_pattern)
	local dlls = table.translate(fullpath_dlls, path.getname)
	
	local dotNetLibs = {
		"System",
		"System.Core",
		"System.Data",
		"System.Net.dll",
		"System.Xml.dll",
		"System.Xml.Linq"
	}
	table.insertflat(dlls, dotNetLibs)
	links(dlls)
	
	-- visual studio copies c# dependencies by default
	-- filter "toolset:not dotnet"
		-- postbuildcommands { "{COPYDIR} \"%{ProjectDir.ScriptCore}/Packages\" \"%{cfg.targetdir}\"" }
	
	filter "configurations:Debug"
		runtime "Debug"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"