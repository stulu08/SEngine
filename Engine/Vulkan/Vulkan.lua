
project "Vulkan"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")

	debugdir ("" .. BuildDir .. "/Vulkan")

	defines
	{
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
		"%{Dependencies}",
		"%{IncludeDir.Stulu}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.mono}",
	}
	links
	{
		"Stulu", 
	}
	prebuildcommands {
		"{RMDIR} \"" .. BuildDir .. "/Vulkan/\"",
		-- copy shaders
		"{RMDIR} \"%{ProjectDir.Vulkan}/LooseFiles/Data/Stulu/Shader/\"",
		"{MKDIR} \"%{ProjectDir.Vulkan}/LooseFiles/Data/Stulu/Shader/\"",
		"{COPYDIR} \"%{ProjectDir.Vulkan}/src/Shader/\" \"%{ProjectDir.Vulkan}/LooseFiles/Data/Stulu/Shader/\"",
	}

	postbuildcommands {
		"{MKDIR} \"" .. BuildDir .. "/Vulkan/Data/Stulu/Vulkan\"",
		-- copy vulkan and engine files
		"{COPYDIR} \"%{ProjectDir.Vulkan}/LooseFiles\" \"" .. BuildDir .. "/Vulkan\"",
		"{COPYDIR} \"%{ProjectDir.Stulu}/LooseFiles\" \"" .. BuildDir .. "/Vulkan\"",
	}

	filter "system:windows"
		systemversion "latest"
		postbuildcommands ("{COPY} \"%{cfg.targetdir}/Vulkan.exe\" \"" .. BuildDir .. "/Vulkan\"")

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
