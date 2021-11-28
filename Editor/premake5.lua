project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	if(staticRuntime) then
		staticruntime "on"
	end

	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")
	debugdir ("" .. builddir .. "")

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
		"%{IncludeDir.Discord}"
	}
	postbuildcommands {
		"{MKDIR} ".. builddir .."",
		"{COPY} %{cfg.targetdir}/Editor.exe " .. builddir .. "",
		"{COPY} %{ProjectDir.Discord}/bin/" .. outputdir .. "/discord-rpc.dll " .. builddir .. ""
	}
	if(staticBuild == false) then
		postbuildcommands
		{
			"{COPY} %{ProjectDir.Stulu}/bin/" .. outputdir .. "/Stulu.dll " .. builddir .. ""
		}
	end
	links
	{
		"Stulu", 
		"discord-rpc",
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
		defines "ST_DIST"
		runtime "Release"
		optimize "on"
