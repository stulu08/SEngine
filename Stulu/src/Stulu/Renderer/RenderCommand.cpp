#include "st_pch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRenderAPI.h"

namespace Stulu {
	Scope<RenderAPI> RenderCommand::s_renderAPI = createScope<OpenGLRenderAPI>();
}