project "Runtime"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	location "%{ObjectDir}"
	targetname ("Runtime");
	
	targetdir ("%{TargetDir}/" .. outputdir .. "/Runtime")
	objdir ("%{ObjectDir}/" .. outputdir .. "/Runtime")

	debugdir ("" .. RuntimeBuildDir .. "")

	defines
	{
		"ST_RUNTIME",
		"ST_DYNAMIC_LINK",
		"APP_DYNAMIC_LINK",
		"_CRT_SECURE_NO_WARNINGS"
	}

	files
	{
		"%{RuntimeDir}/**.c",
		"%{RuntimeDir}/**.cpp",
		"%{RuntimeDir}/**.h",
		"%{RuntimeDir}/**.inl",
		"%{RuntimeDir}/**.hpp",
		
		"%{RuntimeDir}/**.glsl",
		"%{RuntimeDir}/**.comp"
	}

	includedirs {
		"%{RuntimeDir}",
		"%{RuntimeDir}/Include",
		"%{AssetDir}",
		"%{AssetDir}/Include",

		"%{Dependencies}",
		"%{IncludeDir.Stulu}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.Discord}",
	}
	
	links {
		"Native Assembly",
		"%{Library.Stulu}",

		"ImGui",
		"yaml-cpp",
		"Discord C++ Game SDK",
	}
	
	libdirs {
		"%{LibraryDir.StuluNative}"
	}

	prebuildcommands {
		"{RMDIR} \"" .. RuntimeBuildDir .. "/\"",
	}

	postbuildcommands {
		"{MKDIR} \"" .. RuntimeBuildDir .. "\"",
		-- copy discord game sdk dll
		"{COPYFILE} \"%{LibraryDir.Discord}/%{Library.Discord}\" \"".. RuntimeBuildDir .. "/%{Library.Discord}\"",
		-- copy Stulu.dll
		"{COPYFILE} \"%{LibraryDir.StuluNative}/%{Library.StuluDynamic}\" \"" .. RuntimeBuildDir .. "/%{Library.StuluDynamic}\"",
		-- copy engine files
		"{COPYDIR} \"%{ProjectDir.Stulu}/LooseFiles\" \"" .. RuntimeBuildDir .. "\"",
		-- copy Managed Assembly
		"{COPYDIR} \"%{cfg.targetdir}/../Managed\" \"".. RuntimeBuildDir .. "/Data/Stulu/Managed\"",
		-- copy Native Assembly
		"{COPYDIR} \"%{cfg.targetdir}/../Native\" \"".. RuntimeBuildDir .. "\"",
		-- copy Runtime Binaries
		"{COPYDIR} \"%{cfg.targetdir}/\" \"".. RuntimeBuildDir .. "\"",
		-- copy Assets
		"{COPYDIR} \"".. AssetDir .. "/\" \"".. RuntimeBuildDir .. "\"/Data/Assets",
	}


	filter "system:windows"
		systemversion "latest"

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