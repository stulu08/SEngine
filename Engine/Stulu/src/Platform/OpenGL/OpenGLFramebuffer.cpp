#include "st_pch.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLTexture.h"
#include "glad/glad.h"

namespace Stulu {
	GLenum getDepthAttachmentTypeByFormat(TextureFormat format) {
		switch (format)
		{
		case Stulu::TextureFormat::Depth16:
			return GL_DEPTH_ATTACHMENT;
		case Stulu::TextureFormat::Depth24:
			return GL_DEPTH_ATTACHMENT;
		case Stulu::TextureFormat::Depth32:
			return GL_DEPTH_ATTACHMENT;
		case Stulu::TextureFormat::Depth32F:
			return GL_DEPTH_ATTACHMENT;
		case Stulu::TextureFormat::Depth24_Stencil8:
			return GL_DEPTH_STENCIL_ATTACHMENT;
		case Stulu::TextureFormat::Depth32F_Stencil8:
			return GL_DEPTH_STENCIL_ATTACHMENT;
		case Stulu::TextureFormat::Stencil8:
			return GL_STENCIL_ATTACHMENT;
		}
		return GL_DEPTH_ATTACHMENT;
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FrameBufferSpecs& frameBufferdata)
		: m_specs(frameBufferdata) {
		ST_PROFILING_FUNCTION();
		m_texture = std::make_shared<OpenGLFrameBufferTexture>(m_specs);
		invalidate();
	}
	OpenGLFramebuffer::~OpenGLFramebuffer() {
		ST_PROFILING_FUNCTION();
		glDeleteFramebuffers(1, &m_rendererID);
	}
	void OpenGLFramebuffer::bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		glViewport(0, 0, m_specs.width, m_specs.height);
	}
	void OpenGLFramebuffer::unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFramebuffer::invalidate() {
		ST_PROFILING_FUNCTION();
		if (m_rendererID) {
			glDeleteFramebuffers(1, &m_rendererID);
		}
		
		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		m_texture->invalidate();

		//glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, m_specs.samples);

		CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void OpenGLFramebuffer::resize(uint32_t width, uint32_t height) {
		ST_PROFILING_FUNCTION();
		m_specs.width = width;
		m_specs.height = height;
		m_texture->resize(m_specs);
		invalidate();
	}
	void OpenGLFramebuffer::attachCubeMapFace(const Ref<CubeMap>& cubemap, uint32_t face, uint32_t mip) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+face, (int)*cubemap, mip);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
	}
	/////////////////////////////////////////////////////////////////////////////
	OpenGLFrameBufferTexture::~OpenGLFrameBufferTexture() {
		ST_PROFILING_FUNCTION();
		glDeleteTextures(1, &m_colorAttachment);
		glDeleteTextures(1, &m_depthAttachment);
	}
	void OpenGLFrameBufferTexture::invalidate() {
		ST_PROFILING_FUNCTION();
		if(m_depthAttachment)
			glDeleteTextures(1, &m_depthAttachment);
		if (m_colorAttachment)
			glDeleteTextures(1, &m_colorAttachment);
		bool multisampled = m_specs.samples > 1;
		GLenum target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

		if(m_specs.colorTexture.format != TextureFormat::None) {
			TextureSettings& settings = m_specs.colorTexture;

			auto[internalFormat, dataFormat] = TextureFormatToGLenum(settings.format);
			GLenum wrap = TextureWrapToGLenum(settings.wrap);
			bool hasMips = settings.forceGenMips;
			settings.levels = 1;

			glCreateTextures(target, 1, &m_colorAttachment);
			glBindTexture(target, m_colorAttachment);

			if (multisampled) {
				glTexImage2DMultisample(target, m_specs.samples, internalFormat, m_specs.width, m_specs.height, GL_FALSE);
			}
			else {
				glTexImage2D(target, 0, internalFormat, m_specs.width, m_specs.height, 0, dataFormat, isGLTextureFormatFloat(settings.format) ? GL_FLOAT : GL_UNSIGNED_BYTE, nullptr);
			}

			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, TextureFilteringToGLenum(settings.filtering, hasMips, 0));
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, TextureFilteringToGLenum(settings.filtering, hasMips, 1));
			glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
			glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);
			if (settings.wrap == TextureWrap::ClampToBorder) {
				float borderColor[] = { settings.border.x,  settings.border.y,  settings.border.z,  settings.border.w };
				glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
			}

			if (hasMips) {
				float maxAnistropy;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnistropy);
				glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY, maxAnistropy);
				glGenerateMipmap(target);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, m_colorAttachment, 0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
		}
		else {
			glDrawBuffer(GL_NONE);
		}

		if (m_specs.depthTexture.format != TextureFormat::None) {
			TextureSettings& settings = m_specs.depthTexture;

			auto [internalFormat, dataFormat] = TextureFormatToGLenum(settings.format);
			GLenum wrap = TextureWrapToGLenum(settings.wrap);
			bool hasMips = settings.forceGenMips;
			settings.levels = 1;

			glCreateTextures(target, 1, &m_depthAttachment);
			glBindTexture(target, m_depthAttachment);

			if (multisampled) {
				glTexImage2DMultisample(target, m_specs.samples, internalFormat, m_specs.width, m_specs.height, GL_FALSE);
			}
			else {
				glTexStorage2D(target, 1, internalFormat, m_specs.width, m_specs.height);
			}

			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, TextureFilteringToGLenum(settings.filtering, hasMips, 0));
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, TextureFilteringToGLenum(settings.filtering, hasMips, 1));
			glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
			glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);
			if (settings.wrap == TextureWrap::ClampToBorder) {
				float borderColor[] = { settings.border.x,  settings.border.y,  settings.border.z,  settings.border.w };
				glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
			}
			
			if (hasMips) {
				float maxAnistropy;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnistropy);
				glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY, maxAnistropy);
				glGenerateMipmap(target);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, getDepthAttachmentTypeByFormat(settings.format), target, m_depthAttachment, 0);

		}
		updateTextures();
	}
	void OpenGLFrameBufferTexture::resize(FrameBufferSpecs& specs) {
		ST_PROFILING_FUNCTION();
		m_specs = specs;
	}
	void OpenGLFrameBufferTexture::bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_colorAttachment);
	}
	void OpenGLFrameBufferTexture::bindDepthAttachment(uint32_t slot) const {
		glBindTextureUnit(slot, m_depthAttachment);
	}
	void OpenGLFrameBufferTexture::updateTextures() {
		if (!m_depthTexture) {
			m_depthTexture = createRef<OpenGLTexture2D>(m_depthAttachment, m_specs.width, m_specs.height, m_specs.depthTexture);
		}
		else {
			m_depthTexture->updateInternal(m_depthAttachment, m_specs.width, m_specs.height, m_specs.depthTexture);
		}
		if (!m_colorTexture) {
			m_colorTexture = createRef<OpenGLTexture2D>(m_colorAttachment, m_specs.width, m_specs.height, m_specs.colorTexture);
		}
		else {
			m_colorTexture->updateInternal(m_colorAttachment, m_specs.width, m_specs.height, m_specs.colorTexture);
		}
		if(m_colorTexture)
			m_colorTexture->setSettings(this->m_specs.colorTexture);
		if (m_depthTexture)
			m_depthTexture->setSettings(this->m_specs.depthTexture);
	}
}