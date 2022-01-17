project "Hub"
	location "Hub"
	kind "WindowedApp"
	language "C#"
	targetname "Hub"
	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")
	files
	{
		"Hub/AssemblyInfo.cs",
		"Hub/app.manifest",
		"Hub/*.cs",
		"Hub/*.xaml"
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

project "Installer"
	location "Installer"
	kind "WindowedApp"
	language "C#"
	targetname "Installer"
	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")
	files
	{
		"Installer/AssemblyInfo.cs",
		"Installer/app.manifest",
		"Installer/*.cs",
		"Installer/*.xaml"
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
