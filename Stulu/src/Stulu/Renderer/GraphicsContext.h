#pragma once
#include "Stulu/Core/Window.h"

namespace Stulu {
	class STULU_API GraphicsContext {
	public:
		virtual ~GraphicsContext() = default;

		virtual void init(Window* window) = 0;
		virtual void swapBuffers() = 0;
		virtual void setVSync(bool enabled) = 0;


		static Scope<GraphicsContext> create();
	};
}