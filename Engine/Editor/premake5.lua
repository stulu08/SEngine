project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetname ("Stulu Editor");
	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")
	dependson { "EditorScriptCore","Runtime" }

	debugdir ("" .. builddir .. "/Editor")
	debugargs { "%{wks.location}/Projects/DebugProject/DebugProject.sproj" }

	defines
	{
		"ST_EDITOR",
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
		"src/Shader/**",
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
		"%{IncludeDir.mono}",
		"%{IncludeDir.Discord}",
		"%{IncludeDir.premake}",
	}
	prebuildcommands {
		"{RMDIR} \"" .. builddir .. "/Editor/\"",
		"{RMDIR} \"%{ProjectDir.Editor}/LooseFiles/Data/Stulu/Shader/Stulu/Editor\"",


		"{MKDIR} \"%{ProjectDir.Editor}/LooseFiles/Data/Stulu/Shader/Stulu/Editor\"",
		"{COPYDIR} \"%{ProjectDir.Editor}/src/Shader/Stulu/Editor\" \"%{ProjectDir.Editor}/LooseFiles/Data/Stulu/Shader/Stulu/Editor\"",
	}

	postbuildcommands {
		"{MKDIR} \"" .. builddir .. "/Editor/Data/Stulu/Editor\"",
		"{MKDIR} \"" .. builddir .. "/Editor/Data/Stulu/Runtime\"",
		"{MKDIR} \"" .. builddir .. "/Editor/Data/Stulu/Managed\"",

		"{COPY} \"%{dependencies}/discord-rpc/bin/" .. outputdir .. "/discord-rpc.dll\" \"" .. builddir .. "/Editor\"",

		"{COPYDIR} \"%{ProjectDir.Editor}/LooseFiles\" \"" .. builddir .. "/Editor\"",
		"{COPYDIR} \"%{ProjectDir.Stulu}/LooseFiles\" \"" .. builddir .. "/Editor\"",
		
		"{COPYDIR} \"%{ProjectDir.ScriptCore}/bin/".. outputdir .."\" \"" .. builddir .. "/Editor/Data/Stulu/Managed\"",
		"{COPYDIR} \"%{ProjectDir.EditorScriptCore}/bin/".. outputdir .."\" \"" .. builddir .. "/Editor/Data/Stulu/Managed\"",

		"{COPYFILE} \"%{ProjectDir.Runtime}/bin/" .. outputdir .. "/Stulu Runtime.exe\" \"" .. builddir .. "/Editor/Data/Stulu/Runtime/Stulu Runtime.exe\"",
	}
	links
	{
		"Stulu", 
		"Premake5", 
		"discord-rpc"
	}
	filter "system:windows"
		systemversion "latest"
		postbuildcommands ("{COPY} \"%{cfg.targetdir}/Stulu Editor.exe\" \"" .. builddir .. "/Editor\"")

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
