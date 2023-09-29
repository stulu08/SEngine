project "Stulu"
	if(staticBuild) then
		kind "StaticLib"
	else
		kind "SharedLib"
	end
	language "C++"
	cppdialect "C++17"
	if(staticRuntime) then
		staticruntime "on"
	end

	targetdir ("bin/" .. outputdir .. "")
	objdir ("bin-int/" .. outputdir .. "")
	
	dependson { "Stulu-ScriptCore" }
	
	pchheader "st_pch.h"
	pchsource "src/st_pch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
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
	}
	libdirs 
	{ 
		"%{vulkanSDK}/Lib",
		"%{monoDir}/lib"
	}
	
	postbuildcommands{
		"{MKDIR} \"%{ProjectDir.Stulu}/LooseFiles/mono\"",
		"{MKDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data\"",
		"{MKDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/PhysX\"",

		"{COPY} \"%{monoDir}/bin/mono-2.0-sgen.dll\" \"%{ProjectDir.Stulu}/LooseFiles/\"",
		"{COPYDIR} \"%{monoDir}/lib/mono/4.5\" \"%{ProjectDir.Stulu}/LooseFiles/mono/4.5\"",
		"{COPYDIR} \"%{monoDir}/lib/mono/4.8-Api\" \"%{ProjectDir.Stulu}/LooseFiles/mono/4.8-Api\"",

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
