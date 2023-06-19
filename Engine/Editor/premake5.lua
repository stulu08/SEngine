project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	if(staticRuntime) then
		staticruntime "on"
	end
	targetname ("Stulu Editor");
	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")
	debugdir ("" .. builddir .. "")
	debugargs { "%{wks.location}/Projects/DebugProject/DebugProject.sproj" }
	dependson { "EditorScriptCore","Runtime" }
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
		"src/**.h",
		"src/**.cpp",
		"Stulu/**.glsl"
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
	}
	postbuildcommands {
		"{MKDIR} ".. builddir .."/Build",

		"{COPY} %{dependencies}/discord-rpc/bin/" .. outputdir .. "/discord-rpc.dll " .. builddir .. "",

		"{COPYDIR} %{ProjectDir.Editor}/LooseFiles " .. builddir .. "",
		"{COPYDIR} %{ProjectDir.Stulu}/LooseFiles " .. builddir .. "",
		
		"{COPYDIR} %{ProjectDir.ScriptCore}/bin/".. outputdir .." " .. builddir .. "/Data/Managed",
		"{COPYDIR} %{ProjectDir.EditorScriptCore}/bin/".. outputdir .." " .. builddir .. "/Data/Managed",

		"{COPYFILE} \"%{ProjectDir.Runtime}/bin/" .. outputdir .. "/Stulu Runtime.exe\" \"" .. builddir .. "/Build/Stulu Runtime.exe\"",
	}
	links
	{
		"Stulu", 
		"discord-rpc"
	}
	filter "system:windows"
		systemversion "latest"
		postbuildcommands ("{COPY} \"%{cfg.targetdir}/Stulu Editor.exe\" " .. builddir .. "")

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
