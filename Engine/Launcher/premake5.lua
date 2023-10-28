project "Launcher"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	if(staticRuntime) then
		staticruntime "on"
	end
	targetname ("Stulu Launcher");
	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")
	debugdir ("" .. builddir .. "/Launcher")
	defines
	{
		"ST_RUNTIME",
		"_CRT_SECURE_NO_WARNINGS"
	}
	if(staticBuild == false) then
		defines
		{
			"ST_DYNAMIC_LINK",
		}
	end
	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"src",
		"%{dependencies}",
		"%{IncludeDir.Stulu}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.mono}"
	}
	links
	{
		"Stulu"
	}
	postbuildcommands {
		"{MKDIR} \"".. builddir .."/Launcher/Licenses\"",
		"{MKDIR} \"".. builddir .."/Launcher/Textures\"",
		"{COPYDIR} \"%{ProjectDir.Stulu}/LooseFiles/Licenses\" \"" .. builddir .. "/Launcher/Licenses\"",
		"{COPYDIR} \"%{ProjectDir.Stulu}/LooseFiles/Stulu/Textures\" \"" .. builddir .. "/Launcher/Textures\"",
	}
	filter "system:windows"
		systemversion "latest"
		postbuildcommands ("{COPY} \"%{cfg.targetdir}/Stulu Launcher.exe\" \"" .. builddir .. "/Launcher\"")
		linkoptions { "/MANIFESTUAC:\"level='highestAvailable'\"" }

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
