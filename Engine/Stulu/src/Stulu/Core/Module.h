#pragma once
#include "ApplicationInfo.h"
#include "Layer.h"

namespace Stulu {
	// Basicly just an extion to a layer, a module should be loaded from a library or be one of the base modules
	// In contrast to layers, modules should be implemented independet, more details inside layer definition
	class STULU_API Module : public Layer {
	public:
		inline Module(const std::string& path) 
			: Layer(path) {}
		
		virtual ~Module() = default;

		// This is called before everything eles is loaded (UI, Renderer, etc, ...)
		virtual bool onLoad(const ApplicationInfo& appInfo) { return true; }

		inline std::string GetPath() const { return Layer::getName(); }
		inline Layer* GetLayer() const { return (Layer*)this; }

		static void LoadBaseModules();
	};


	// pre compile time module properties
	struct ModuleOverlay {
		static constexpr bool IsOverlay = true;
	};
	struct ModuleDefault {
		static constexpr bool IsOverlay = false;
	};


	template<class T, class = void>
	struct ModuleInfo {
		using Module = T;
		static constexpr bool IsOverlay = false;
	};
	template<class T>
	struct ModuleInfo<T, std::void_t<typename T::ModuleType>> {
		using Module = T;
		static constexpr bool IsOverlay = Module::ModuleType::IsOverlay;
	};
}