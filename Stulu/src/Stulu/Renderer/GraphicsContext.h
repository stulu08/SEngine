#pragma once

namespace Stulu {
	class GraphicsContext {
	public:
		virtual ~GraphicsContext() = default;

		virtual void init() = 0;
		virtual void swapBuffers() = 0;
	};
}