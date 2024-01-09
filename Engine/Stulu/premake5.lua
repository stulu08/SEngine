project "Stulu"
	if(staticBuild) then
		kind "StaticLib"
	else
		kind "SharedLib"
	end
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")
	
	dependson { "Stulu-ScriptCore" }
	
	pchheader "st_pch.h"
	pchsource "src/st_pch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/Shader/**",

		"%{dependencies}/stb_image/**.cpp",
		"%{dependencies}/stb_image/**.h",
		"%{dependencies}/glm/glm/**.hpp",
		"%{dependencies}/glm/glm/**.inl",
		"%{dependencies}/ReputelessPerlinNoise/**.h"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
	}
	if(staticBuild == false) then
		defines
		{
			"ST_DYNAMIC_LINK",
			"ST_DLL_BUILD"
		}
	end

	includedirs
	{
		"src",
		"%{dependencies}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.noise}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.PhysX}",
		"%{IncludeDir.mono}",
		
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"assimp",
		"yaml-cpp",
		"ImGuizmo",
		
		"PhysX",
		"PhysXCommon",
		"PhysXFoundation",
		"PhysXPvdSDK",
		"PhysXExtensions",
		"PhysXCooking",
		"PhysXCharacterKinematic",
		
		"opengl32.lib",

		"vulkan-1.lib",
		"VkLayer_utils.lib",
		
		"mono-2.0-sgen.lib",
		--"libmono-static-sgen.lib",
	}
	libdirs 
	{ 
		"%{vulkanSDK}/Lib",
		"%{monoDir}/lib"
	}
	
	prebuildcommands {
		"{RMDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/mono\"",
		"{RMDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/PhysX\"",
		"{RMDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/Stulu/Shader\"",


		"{MKDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/Stulu/Shader\"",
		"{COPYDIR} \"%{ProjectDir.Stulu}/src/Shader\" \"%{ProjectDir.Stulu}/LooseFiles/Data/Stulu/Shader\"",
	}

	postbuildcommands {
		"{MKDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data\"",
		"{MKDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/PhysX\"",
		"{MKDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/mono\"",

		"{COPY} \"%{monoDir}/bin/mono-2.0-sgen.dll\" \"%{ProjectDir.Stulu}/LooseFiles/\"",
		"{COPYDIR} \"%{monoDir}/etc\" \"%{ProjectDir.Stulu}/LooseFiles/Data/mono/etc\"",
		"{COPYDIR} \"%{monoDir}/lib/\" \"%{ProjectDir.Stulu}/LooseFiles/Data/mono/lib/\"",

		"{COPYDIR} \"%{physxDir}/loader/" .. outputdir .. "\" \"%{ProjectDir.Stulu}/LooseFiles/Data/PhysX\"",

	}
	
	
	filter "system:windows"
		systemversion "latest"
		defines
		{
			"GLFW_INCLUDE_NONE"
		}

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
		runtime "Release"
		optimize "on"
		symbols "off"
