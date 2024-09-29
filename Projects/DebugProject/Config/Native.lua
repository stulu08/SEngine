project "Native Assembly"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	location "%{ObjectDir}"
	targetname "NativeAssembly"
	
	targetdir ("%{TargetDir}/" .. outputdir .. "/Native")
	objdir ("%{ObjectDir}/" .. outputdir .. "/Native")
	
	defines {
		"ST_DYNAMIC_LINK",
		"APP_DLL_BUILD",
		"APP_DYNAMIC_LINK",
		"_CRT_SECURE_NO_WARNINGS",
	}
	
	files {
		"%{AssetDir}/**.c",
		"%{AssetDir}/**.cpp",
		"%{AssetDir}/**.h",
		"%{AssetDir}/**.inl",
		"%{AssetDir}/**.hpp",
		
		"%{AssetDir}/**.glsl",
		"%{AssetDir}/**.comp"
	}
	
	includedirs {
		"%{AssetDir}",
		"%{AssetDir}/Include",

		"%{Dependencies}",
		"%{IncludeDir.Stulu}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.Discord}",
	}
	
	links {
		"%{Library.Stulu}",

		"ImGui",
		"yaml-cpp",
		"Discord C++ Game SDK",
	}
	
	libdirs {
		"%{LibraryDir.StuluNative}",
	}

	filter "system:windows"
		systemversion "latest"
		defines "ST_WINDOWS"

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