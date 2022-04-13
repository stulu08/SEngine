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
		"vendor/stb_image/**.cpp",
		"vendor/stb_image/**.h",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/ReputelessPerlinNoise/**.h"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
	}
	if(staticBuild == false) then
		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"ST_DYNAMIC_LINK",
			"ST_DLL_BUILD"
		}
	end

	includedirs
	{
		"src",
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
		"%{IncludeDir.PhysX}/physx",
		"%{IncludeDir.mono}/mono-2.0",
		
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"assimp",
		"yaml-cpp",
		"ImGuizmo",
		
		"opengl32.lib",

		"vulkan-1.lib",
		"VkLayer_utils.lib",
		
		"mono-2.0-sgen.lib",
		
		"PhysX_static_64.lib",
		"PhysXCommon_static_64.lib",
		"PhysXFoundation_static_64.lib",
		"PhysXPvdSDK_static_64.lib",
		"PhysXExtensions_static_64.lib",
		"PhysXCooking_static_64.lib"
	}
	libdirs 
	{ 
		"%{vulkanSDK}/Lib",
		"%{monoDir}/lib"
	}
	if(staticRuntime) then
		libdirs
		{
			"%{physx}/bin/static/".. outputdir ..""
		}
	else
		libdirs
		{
			"%{physx}/bin/dynamic/".. outputdir ..""
		}
	end
	
	postbuildcommands{
		"{MKDIR} %{ProjectDir.Stulu}/bin/".. outputdir .."/data/PhysX",
		"{COPYDIR} %{physx}/bin/dll/".. outputdir .." %{ProjectDir.Stulu}/bin/" .. outputdir .. "/data/PhysX",
		
		"{COPY} \"%{monoDir}/bin/mono-2.0-sgen.dll\" \"%{ProjectDir.Stulu}/bin/" .. outputdir .. "\"",
		"{COPYDIR} \"%{monoDir}/lib/mono/4.5\" \"%{ProjectDir.Stulu}/bin/" .. outputdir .. "/mono/4.5\"",
		"{COPYDIR} \"%{monoDir}/lib/mono/4.8-Api\" \"%{ProjectDir.Stulu}/bin/" .. outputdir .. "/mono/4.8-Api\"",
		"{COPYDIR} %{ProjectDir.ScriptCore}/bin/".. outputdir .." %{ProjectDir.Stulu}/bin/" .. outputdir .. "/data",
		"{COPYDIR} %{ProjectDir.EditorScriptCore}/bin/".. outputdir .." %{ProjectDir.Stulu}/bin/" .. outputdir .. "/data",
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
		symbols "on"

	filter "configurations:Release"
		defines "ST_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ST_DIST"
		runtime "Release"
		optimize "on"
