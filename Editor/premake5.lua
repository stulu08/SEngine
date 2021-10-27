project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	defines
	{
		"ST_EDITOR",
		"_CRT_SECURE_NO_WARNINGS"
	}
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

	links
	{
		"Stulu", 
		"discord-rpc"
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