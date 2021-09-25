#include "st_pch.h"
#include "OpenGLFramebuffer.h"

#include "glad/glad.h"

namespace Stulu {
	OpenGLFramebuffer::OpenGLFramebuffer(const FrameBufferSpecs& frameBufferdata)
		: m_specs(frameBufferdata) {
		ST_PROFILING_FUNCTION();
		Invalidate();
	}
	OpenGLFramebuffer::~OpenGLFramebuffer() {
		ST_PROFILING_FUNCTION();
		glDeleteFramebuffers(1, &m_rendererID);
		glDeleteTextures(1, &m_colorAttachment);
		glDeleteTextures(1, &m_depthAttachment);
	}
	void OpenGLFramebuffer::bind() const {
		ST_PROFILING_FUNCTION();
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		glViewport(0, 0, m_specs.width, m_specs.height);
	}
	void OpenGLFramebuffer::unBind() const {
		ST_PROFILING_FUNCTION();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFramebuffer::Invalidate() {
		ST_PROFILING_FUNCTION();
		if (m_rendererID) {
			glDeleteFramebuffers(1, &m_rendererID);
			glDeleteTextures(1, &m_colorAttachment);
			glDeleteTextures(1, &m_depthAttachment);
		}

		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_specs.width, m_specs.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_specs.width, m_specs.height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment, 0);

		glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, m_specs.samples);

		CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height) {
		ST_PROFILING_FUNCTION();
		m_specs.width = width;
		m_specs.height = height;
		Invalidate();
	}
	FrameBufferSpecs& OpenGLFramebuffer::getSpecs() {
		return m_specs;
	}
}