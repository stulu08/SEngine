#include "st_pch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRenderAPI.h"

namespace Stulu {
	RenderAPI* RenderCommand::s_renderAPI = new OpenGLRenderAPI();
}