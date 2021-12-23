project "Launcher"
	kind "WindowedApp"
	language "C#"
	targetname "Launcher"
	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")
	files
	{
		"AssemblyInfo.cs",
		"app.manifest",
		"*.cs",
		"*.xaml"
	}
	flags { "WPF" }
	nuget { "Microsoft.WindowsAPICodePack-Shell:1.1.0" }
	links {
		"Microsoft.Csharp",
		"PresentationCore",
		"PresentationFramework",
		"System",
		"System.Core",
		"System.Data",
		"System.Data.DataSetExtensions",
		"System.IO.Compression.FileSystem",
		"System.Net.Http",
		"System.Windows.Forms",
		"System.Xaml",
		"System.Xml",
		"System.Xml.Linq",
		"WindowsBase"
	}
	filter "configurations:Debug"
		runtime "Debug"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
