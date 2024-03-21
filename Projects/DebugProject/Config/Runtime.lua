project "Runtime"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	location "%{ObjectDir}"
	targetname ("Runtime");
	
	targetdir ("%{TargetDir}/" .. outputdir .. "")
	objdir ("%{ObjectDir}/Native/" .. outputdir .. "")

	defines
	{
		"ST_RUNTIME",
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

	includedirs
	{
		"%{RuntimeDir}",
		"%{RuntimeDir}/Include",
		"%{AssetDir}",
		"%{AssetDir}/Include",
		"%{Dependencies}",
		"%{IncludeDir.Stulu}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.mono}"
	}
	links {
		"Native Assembly",
		
		"%{Library.Stulu}"
		
	}
	
	libdirs {
		"%{LibraryDir.StuluNative}"
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