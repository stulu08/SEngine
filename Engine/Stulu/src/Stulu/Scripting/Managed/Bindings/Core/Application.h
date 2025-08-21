#pragma once
#include "Stulu/Core/Application.h"

namespace StuluBindings {
	class Application {
	public:
		static inline void exit(int32_t code) {
			return Stulu::Application::get().exit(code);
		}
		static inline uint32_t getWidth() {
			return Stulu::Application::get().getWidth();
		}
		static inline uint32_t getHeight() {
			return Stulu::Application::get().getHeight();
		}
	};
}