#pragma once
#include "Stulu/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Stulu {
	class STULU_API OpenGLContext : public GraphicsContext {
	public:
		OpenGLContext();

		virtual void init(Window* window) override;
		virtual void beginBuffer() override;
		virtual void swapBuffers() override;
		virtual void setVSync(bool enabled) override;
		virtual API_Infos getApiInfos() override;
	private:
		GLFWwindow* m_windowHandle;
	};
}


