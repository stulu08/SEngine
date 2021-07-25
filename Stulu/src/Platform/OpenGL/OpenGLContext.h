#pragma once
#include "Stulu/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Stulu {
	class OpenGLContext : public GraphicsContext {
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void init() override;
		virtual void swapBuffers() override;

	private:
		GLFWwindow* m_windowHandle;
	};
}


