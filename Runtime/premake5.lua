project "Runtime"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	if(staticRuntime) then
		staticruntime "on"
	end
	targetname ("Stulu Runtime");
	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")
	debugdir ("" .. builddir .. "-Runtime")
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
		"%{wks.location}/Stulu/src",
		"%{wks.location}/Stulu/vendor",
		"src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.mono}/mono-2.0"
	}
	postbuildcommands {
		"{MKDIR} ".. builddir .."-Runtime",
		"{COPYDIR} %{ProjectDir.Runtime}/Files " .. builddir .. "-Runtime",
		"{COPYDIR} %{ProjectDir.Stulu}/bin/" .. outputdir .. " " .. builddir .. "-Runtime",
		"{DELETE} " .. builddir .. "-Runtime/Stulu.lib",
		"{DELETE} " .. builddir .. "-Runtime/Stulu.idb",
		"{DELETE} " .. builddir .. "-Runtime/Stulu.pdb",
		"{DELETE} " .. builddir .. "-Runtime/data/Managed/Stulu.ScriptCore.pdb",
		"{DELETE} " .. builddir .. "-Runtime/data/Managed/Stulu.EditorScriptCore.pdb",
		"{DELETE} " .. builddir .. "-Runtime/data/Managed/Stulu.EditorScriptCore.dll",
	}
	links
	{
		"Stulu",
		"ImGuizmo",
		"ImGui",

	}
	filter "system:windows"
		systemversion "latest"
		postbuildcommands ("{COPY} \"%{cfg.targetdir}/Stulu Runtime.exe\" " .. builddir .. "-Runtime")

	filter "configurations:Debug"
		defines "ST_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ST_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ST_DIST"
		runtime "Release"
		optimize "on"
