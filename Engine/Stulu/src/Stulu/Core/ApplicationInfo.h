#pragma once
#include "Stulu/Types/Version.h"
#include "Stulu/Renderer/Window.h"
#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
	struct ApplicationInfo {
		std::string Name;
		std::string Publisher;
		Version Version;
		WindowProps WindowProps;

		std::string DataPath = "Data/";
		std::string AppPath;
		std::string AppCachePath;
		std::string AppAssetPath;
		std::string AppManagedAssembly;
		std::string AppNativeAssembly;

		Renderer::API api = Renderer::API::OpenGL;

		uint64_t threadPoolSize = std::thread::hardware_concurrency() - 1;

		bool HideWindowOnSart = false;
		bool StartPhysicsEngine = false;
		bool EnableImgui = true;
		// this includes default assets, gizmo and renderer
		bool LoadDefaultAssets = true;
		// include mono debug features
		bool DebugFeatures = false;
	};
}