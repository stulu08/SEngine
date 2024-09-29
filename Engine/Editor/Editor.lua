project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetname ("Stulu Editor");
	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")

	debugdir ("" .. BuildDir .. "/Editor")
	debugargs { "%{ProjectDir.DebugProject}" }

	defines
	{
		"ST_EDITOR",
		"_CRT_SECURE_NO_WARNINGS"
	}

	files
	{
		"src/Shader/**",
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
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.premake}",
		"%{IncludeDir.Discord}",
	}
	links
	{
		"Stulu", 
		"Premake5",
		"ImGui",
		"yaml-cpp",
		"Discord C++ Game SDK",
	}
	prebuildcommands {
		"{RMDIR} \"" .. BuildDir .. "/Editor/\"",
		-- copy shaders
		"{RMDIR} \"%{ProjectDir.Editor}/LooseFiles/Data/Stulu/Shader/Stulu/Editor\"",
		"{MKDIR} \"%{ProjectDir.Editor}/LooseFiles/Data/Stulu/Shader/Stulu/Editor\"",
		"{COPYDIR} \"%{ProjectDir.Editor}/src/Shader/Stulu/Editor\" \"%{ProjectDir.Editor}/LooseFiles/Data/Stulu/Shader/Stulu/Editor\"",
		-- copy discord game sdk dll
		"{COPYFILE} \"%{LibraryDir.Discord}/%{Library.Discord}\" \"%{ProjectDir.Editor}/LooseFiles/%{Library.Discord}\"",
	}

	postbuildcommands {
		"{MKDIR} \"" .. BuildDir .. "/Editor/Data/Stulu/Editor\"",
		-- copy editor and engine files
		"{COPYDIR} \"%{ProjectDir.Editor}/LooseFiles\" \"" .. BuildDir .. "/Editor\"",
		"{COPYDIR} \"%{ProjectDir.Stulu}/LooseFiles\" \"" .. BuildDir .. "/Editor\"",
	}

	if(staticBuild == false) then
		defines
		{
			"ST_DYNAMIC_LINK",
		}
		-- copy Stulu.dll to Editor.exe
		postbuildcommands {
			"{COPYFILE} \"%{LibraryDir.StuluNative}/%{Library.StuluDynamic}\" \"" .. BuildDir .. "/Editor/%{Library.StuluDynamic}\"",
		}
	end

	filter "system:windows"
		systemversion "latest"
		postbuildcommands ("{COPY} \"%{cfg.targetdir}/Stulu Editor.exe\" \"" .. BuildDir .. "/Editor\"")

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
