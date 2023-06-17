project "PhysX"
	kind "StaticLib"
	language "C++"
	if(staticRuntime) then
		staticruntime "on"
	end
	location "%{physxDir}"
	targetdir ("bin/" .. outputdir .."")
	objdir ("bin-int/" .. outputdir .."")

	includedirs {
		"%{physxDir}/include",
		"%{physxDir}/physx/source/common/include",
		"%{physxDir}/physx/source/common/src",
		"%{physxDir}/physx/source/physx/src",
		"%{physxDir}/physx/source/physx/src/device",
		"%{physxDir}/physx/source/physx/src/buffering",
		"%{physxDir}/physx/source/physxgpu/include",
		"%{physxDir}/physx/source/geomutils/include",
		"%{physxDir}/physx/source/geomutils/src",
		"%{physxDir}/physx/source/geomutils/src/contact",
		"%{physxDir}/physx/source/geomutils/src/common",
		"%{physxDir}/physx/source/geomutils/src/convex",
		"%{physxDir}/physx/source/geomutils/src/distance",
		"%{physxDir}/physx/source/geomutils/src/sweep",
		"%{physxDir}/physx/source/geomutils/src/gjk",
		"%{physxDir}/physx/source/geomutils/src/intersection",
		"%{physxDir}/physx/source/geomutils/src/mesh",
		"%{physxDir}/physx/source/geomutils/src/hf",
		"%{physxDir}/physx/source/geomutils/src/pcm",
		"%{physxDir}/physx/source/geomutils/src/ccd",
		"%{physxDir}/physx/source/lowlevel/api/include",
		"%{physxDir}/physx/source/lowlevel/software/include",
		"%{physxDir}/physx/source/lowlevel/common/include/pipeline",
		"%{physxDir}/physx/source/lowlevelaabb/include",
		"%{physxDir}/physx/source/lowleveldynamics/include",
		"%{physxDir}/physx/source/simulationcontroller/include",
		"%{physxDir}/physx/source/simulationcontroller/src",
		"%{physxDir}/physx/source/physxcooking/src",
		"%{physxDir}/physx/source/physxcooking/src/mesh",
		"%{physxDir}/physx/source/physxcooking/src/convex",
		"%{physxDir}/physx/source/scenequery/include",
		"%{physxDir}/physx/source/physxmetadata/core/include",
		"%{physxDir}/physx/source/immediatemode/include",
		"%{physxDir}/physx/source/pvd/include",
		"%{physxDir}/physx/source/foundation/include",
	}
	links {
		"kernel32.lib",
		"user32.lib",
		"gdi32.lib",
		"winspool.lib",
		"shell32.lib",
		"ole32.lib",
		"oleaut32.lib",
		"uuid.lib",
		"comdlg32.lib",
		"advapi32.lib",
		
		"LowLevel",
		"LowLevelAABB",
		"LowLevelDynamics",
		"PhysXCommon",
		"PhysXFoundation",
		"PhysXPvdSDK",
		"PhysXTask",
		"SceneQuery",
		"SimulationController"
	}
	files {
		"%{physxDir}/physx/source/physx/src/**.h",
		"%{physxDir}/physx/source/physx/src/**.cpp",
		"%{physxDir}/physx/source/immediatemode/src/**.cpp",
		"%{physxDir}/physx/source/physxmetadata/core/**.h",
		"%{physxDir}/physx/source/physxmetadata/core/**.cpp",
	}

	defines {
		"WIN32",
		"WIN64",
		"_CRT_SECURE_NO_DEPRECATE",
		"_CRT_NONSTDC_NO_DEPRECATE",
		"_WINSOCK_DEPRECATED_NO_WARNINGS",
		"PX_PHYSX_GPU_SHARED_LIB_NAME=PhysXGpu_64.dll",
		"PX_PHYSX_STATIC_LIB",
		"PX_SUPPORT_PVD=1"
	}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		removefiles { "%{physxDir}/physx/source/physx/src/windows/**.cpp", "%{physxDir}/physx/source/physx/src/device/windows/**.cpp" } 
		
	filter "system:windows"
		systemversion "latest"
		removefiles { "%{physxDir}/physx/source/physx/src/device/linux/**.cpp" } 

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		optimize "off"
		defines {
			"PX_DEBUG=1",
			"PX_CHECKED=1",
			"PX_NVTX=0",
			"_DEBUG"
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		symbols "on"
		defines {
			"_HAS_ITERATOR_DEBUGGING=0",
			"_ITERATOR_DEBUG_LEVEL=0",
			"PX_CHECKED=1",
			"PX_NVTX=0",
			"NDEBUG"
		}

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
		symbols "off"
		defines {
			"_HAS_ITERATOR_DEBUGGING=0",
			"_ITERATOR_DEBUG_LEVEL=0",
			"PX_CHECKED=1",
			"NDEBUG"
		}