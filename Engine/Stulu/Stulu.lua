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
	
	dependson { "ScriptCore" }
	
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
		"%{Dependencies}",
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
		
		"%{Library.OpenGL}",
		"%{Library.Vulkan}",
		"%{Library.VulkanLayerUtils}",
		"%{Library.mono}",
		"Ws2_32.lib", -- mono dependencies
		"Winmm.lib", -- mono dependencies
		"Version.lib", -- mono dependencies
		"Bcrypt.lib", -- mono dependencies
	}
	libdirs 
	{ 
		"%{LibraryDir.Vulkan}",
		"%{LibraryDir.mono}"
	}
	
	prebuildcommands {
		"{MKDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data\"",
	
		"{RMDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/mono\"",
		"{RMDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/PhysX\"",
		"{RMDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/Stulu/Shader\"",
		"{RMDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/Stulu/Native/\"",
		"{RMDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/Stulu/Managed/\"",

		"{MKDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/mono\"",
		"{MKDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/PhysX\"",
		"{MKDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/Stulu/Shader\"",
		"{MKDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/Stulu/Native/\"",
		"{MKDIR} \"%{ProjectDir.Stulu}/LooseFiles/Data/Stulu/Managed/\"",
	}

	postbuildcommands {
		-- copy mono
		"{COPYDIR} \"%{ProjectDir.mono}/etc\" \"%{ProjectDir.Stulu}/LooseFiles/Data/mono/etc\"",
		"{COPYDIR} \"%{ProjectDir.mono}/lib/\" \"%{ProjectDir.Stulu}/LooseFiles/Data/mono/lib/\"",
		-- copy shaders
		"{COPYDIR} \"%{ProjectDir.Stulu}/src/Shader\" \"%{ProjectDir.Stulu}/LooseFiles/Data/Stulu/Shader\"",
		-- copy native binaries
		"{COPYDIR} \"%{cfg.targetdir}\" \"%{ProjectDir.Stulu}/LooseFiles/Data/Stulu/Native/\"",
		-- copy managed binaries
		"{COPYDIR} \"%{ProjectDir.ScriptCore}/bin/".. outputdir .."\" \"%{ProjectDir.Stulu}/LooseFiles/Data/Stulu/Managed/\"",
		-- copy Physx binaries
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
