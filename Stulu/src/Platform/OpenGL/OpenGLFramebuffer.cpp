#include "st_pch.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLTexture.h"
#include "glad/glad.h"

namespace Stulu {
	OpenGLFramebuffer::OpenGLFramebuffer(const FrameBufferSpecs& frameBufferdata)
		: m_specs(frameBufferdata) {
		ST_PROFILING_FUNCTION();
		m_texture = std::make_shared<OpenGLFrameBufferTexture>(m_specs.width, m_specs.height);
		invalidate();
	}
	OpenGLFramebuffer::~OpenGLFramebuffer() {
		ST_PROFILING_FUNCTION();
		glDeleteFramebuffers(1, &m_rendererID);
	}
	void OpenGLFramebuffer::bind() const {
		ST_PROFILING_FUNCTION();
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		glViewport(0, 0, m_specs.width, m_specs.height);
	}
	void OpenGLFramebuffer::unbind() const {
		ST_PROFILING_FUNCTION();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFramebuffer::invalidate() {
		ST_PROFILING_FUNCTION();
		if (m_rendererID) {
			glDeleteFramebuffers(1, &m_rendererID);
		}

		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		m_texture->invalidate(m_specs.textureFormat);

		glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, m_specs.samples);

		CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void OpenGLFramebuffer::resize(uint32_t width, uint32_t height) {
		ST_PROFILING_FUNCTION();
		m_texture->resize(width, height);
		m_specs.width = width;
		m_specs.height = height;
		invalidate();
	}
	FrameBufferSpecs& OpenGLFramebuffer::getSpecs() {
		return m_specs;
	}
	/////////////////////////////////////////////////////////////////////////////
	OpenGLFrameBufferTexture::OpenGLFrameBufferTexture(uint32_t width, uint32_t height) {
		ST_PROFILING_FUNCTION();
		m_width = width;
		m_height = height;
	}
	OpenGLFrameBufferTexture::~OpenGLFrameBufferTexture() {
		ST_PROFILING_FUNCTION();
		glDeleteTextures(1, &m_colorAttachment);
		glDeleteTextures(1, &m_depthAttachment);
	}
	void OpenGLFrameBufferTexture::invalidate(TextureSettings::Format& format) {
		ST_PROFILING_FUNCTION();
		if(m_depthAttachment)
			glDeleteTextures(1, &m_depthAttachment);
		if (m_colorAttachment)
			glDeleteTextures(1, &m_colorAttachment);
		std::pair<GLenum, GLenum> f = TextureFormatToGLenum(format, 4);
		GLenum internalFormat = f.first;
		GLenum dataFormat = f.second;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, dataFormat, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_width, m_height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment, 0);
	}
	void OpenGLFrameBufferTexture::resize(uint32_t width, uint32_t height) {
		ST_PROFILING_FUNCTION();
		m_width = width;
		m_height = height;
	}
	void OpenGLFrameBufferTexture::bind(uint32_t slot) const {
		ST_PROFILING_FUNCTION();
		int32_t active_id = 0;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &active_id);
		if (active_id != m_colorAttachment) {
			glBindTextureUnit(slot, m_colorAttachment);
		}
	}
}