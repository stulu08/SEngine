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
		"{COPYDIR} %{ProjectDir.Stulu}/bin/" .. outputdir .. "/data %{ProjectDir.Runtime}/bin/" .. outputdir .. "/data",
		"{COPYDIR} %{ProjectDir.Runtime}/Files %{ProjectDir.Runtime}/bin/" .. outputdir .. "",
		"{COPYFILE} %{ProjectDir.Stulu}/bin/" .. outputdir .. "/mono-2.0-sgen.dll %{ProjectDir.Runtime}/bin/" .. outputdir .. "/mono-2.0-sgen.dll",
		
		"{DELETE} %{ProjectDir.Runtime}/bin/" .. outputdir .. "/Stulu.lib",
		"{DELETE} %{ProjectDir.Runtime}/bin/" .. outputdir .. "/Stulu.idb",
		"{DELETE} %{ProjectDir.Runtime}/bin/" .. outputdir .. "/Stulu.pdb",

		"{DELETE} \"%{ProjectDir.Runtime}/bin/" .. outputdir .. "/Stulu Runtime.lib\"",
		"{DELETE} \"%{ProjectDir.Runtime}/bin/" .. outputdir .. "/Stulu Runtime.exp\"",
		"{DELETE} \"%{ProjectDir.Runtime}/bin/" .. outputdir .. "/Stulu Runtime.pdb\"",
		
		"{DELETE} %{ProjectDir.Runtime}/bin/" .. outputdir .. "/data/Managed/Stulu.ScriptCore.pdb",
		"{DELETE} %{ProjectDir.Runtime}/bin/" .. outputdir .. "/data/Managed/Stulu.EditorScriptCore.pdb",
		"{DELETE} %{ProjectDir.Runtime}/bin/" .. outputdir .. "/data/Managed/Stulu.EditorScriptCore.dll",
	}
	links
	{
		"Stulu",
		"ImGuizmo",
		"ImGui",

	}
	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "ST_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ST_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		postbuildcommands {
			"{DELETE} \"%{ProjectDir.Runtime}/bin/" .. outputdir .. "/Stulu Runtime.lib\"",
			"{DELETE} \"%{ProjectDir.Runtime}/bin/" .. outputdir .. "/Stulu Runtime.pdb\"",
			"{DELETE} \"%{ProjectDir.Runtime}/bin/" .. outputdir .. "/Stulu Runtime.exp\"",
		}
		defines "ST_DIST"
		runtime "Release"
		optimize "on"
