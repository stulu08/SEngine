#pragma once
#include "Layer.h"

namespace Stulu {
	class STULU_API Module : public Layer {
	public:
		inline Module(const std::string& path) 
			: Layer(path) {}
		
		virtual ~Module() = default;

		virtual void onLoad() {}

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