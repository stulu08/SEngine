project "Launcher"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetname ("Stulu Launcher");
	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")

	debugdir ("" .. BuildDir .. "/Launcher")

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
		"%{Dependencies}",
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
		"{RMDIR} \"" .. BuildDir .. "/Launcher/\"",
		"{MKDIR} \"" .. BuildDir .. "/Launcher/\"",

		-- copy engine files
		"{MKDIR} \"".. BuildDir .."/Launcher/Licenses\"",
		"{MKDIR} \"".. BuildDir .."/Launcher/Textures\"",
		"{COPYDIR} \"%{ProjectDir.Stulu}/LooseFiles/Licenses\" \"" .. BuildDir .. "/Launcher/Licenses\"",
		"{COPYDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/Stulu/Textures\" \"" .. BuildDir .. "/Launcher/Textures\"",
	}
	
	filter "system:windows"
		systemversion "latest"
		postbuildcommands ("{COPY} \"%{cfg.targetdir}/Stulu Launcher.exe\" \"" .. BuildDir .. "/Launcher\"")
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
