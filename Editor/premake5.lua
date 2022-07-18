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
	debugargs { "%{wks.location}/DebugProject/DebugProject.sproj" }
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
		"%{wks.location}/Stulu/src",
		"%{wks.location}/Stulu/vendor",
		"src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.mono}/mono-2.0",
		"%{IncludeDir.Discord}",
	}
	postbuildcommands {
		"{MKDIR} ".. builddir .."/build",

		"{COPY} %{ProjectDir.Discord}/bin/" .. outputdir .. "/discord-rpc.dll " .. builddir .. "",

		"{COPYDIR} %{ProjectDir.Editor}/EditorFiles " .. builddir .. "",

		"{COPYDIR} %{ProjectDir.Stulu}/bin/" .. outputdir .. " " .. builddir .. "",
		"{COPYDIR} %{ProjectDir.ScriptCore}/bin/".. outputdir .." " .. builddir .. "/data/Managed",
		"{COPYDIR} %{ProjectDir.EditorScriptCore}/bin/".. outputdir .." " .. builddir .. "/data/Managed",

		"{COPYFILE} \"%{ProjectDir.Runtime}/bin/" .. outputdir .. "/Stulu Runtime.exe\" \"" .. builddir .. "/build/Stulu Runtime.exe\"",
		"{COPYDIR} %{ProjectDir.Runtime}/Files " .. builddir .. "/build",

		"{DELETE} " .. builddir .. "/Stulu.lib", --we dont need these files and i dont have a lot of space left
		"{DELETE} " .. builddir .. "/Stulu.idb",
		"{DELETE} " .. builddir .. "/Stulu.pdb",
		"{DELETE} " .. builddir .. "/data/Managed/Stulu.ScriptCore.pdb",
		"{DELETE} " .. builddir .. "/data/Managed/Stulu.EditorScriptCore.pdb",
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
