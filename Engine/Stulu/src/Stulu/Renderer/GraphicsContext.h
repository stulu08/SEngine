#pragma once
#include "Stulu/Types/Pointers.h"

#include <string>
namespace Stulu {
	class Window;
	struct API_Infos {
		const std::string name;
		const std::string vendor;
		const std::string version;
		const std::string device;
	};
	class STULU_API GraphicsContext {
	public:
		virtual ~GraphicsContext() = default;

		virtual void init(Window* window) = 0;
		virtual void beginBuffer() = 0;
		virtual void swapBuffers() = 0;
		virtual void setVSync(bool enabled) = 0;

		virtual API_Infos getApiInfos() = 0;

		static Scope<GraphicsContext> create();
	};
}