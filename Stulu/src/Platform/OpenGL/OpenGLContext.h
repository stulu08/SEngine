#pragma once
#include "Stulu/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Stulu {
	class OpenGLContext : public GraphicsContext {
	public:
		OpenGLContext();

		virtual void init(Window* window) override;
		virtual void swapBuffers() override;
		virtual void setVSync(bool enabled) override;

	private:
		GLFWwindow* m_windowHandle;
	};
}


