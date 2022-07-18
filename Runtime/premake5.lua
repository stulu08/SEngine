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
	links
	{
		"Stulu"
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
