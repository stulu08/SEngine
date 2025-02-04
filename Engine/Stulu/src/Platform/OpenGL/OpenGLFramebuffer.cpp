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

	OpenGLFramebuffer::OpenGLFramebuffer(const FrameBufferSpecs& specs)
		: m_specs(specs) {

		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		updateDrawBuffers();

		CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FrameBufferSpecs& specs, const TextureSettings& colorBuffer, const TextureSettings& depthBuffer)
		: m_specs(specs) {

		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		if (colorBuffer.format != TextureFormat::None) {
			m_colorAttachments.push_back(Texture2D::create(specs.width, specs.height, colorBuffer));
			GLenum texId = NativeRenderObjectCast<GLenum>(m_colorAttachments[0]->getNativeRendererObject());
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
		}
		if (depthBuffer.format != TextureFormat::None) {
			m_depthAttachment = Texture2D::create(specs.width, specs.height, depthBuffer);
			GLenum texId = NativeRenderObjectCast<GLenum>(m_depthAttachment->getNativeRendererObject());
			glFramebufferTexture2D(GL_FRAMEBUFFER, getDepthAttachmentTypeByFormat(m_depthAttachment->getSettings().format), GL_TEXTURE_2D, texId, 0);
		}

		updateDrawBuffers();

		CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGLFramebuffer::~OpenGLFramebuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		
		//for (uint32_t i = 0; i < m_colorAttachments.size(); i++) {
		//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 0, 0, 0);
		//}
		//if (m_depthAttachment) {
		//	glFramebufferTexture2D(GL_FRAMEBUFFER, getDepthAttachmentTypeByFormat(m_depthAttachment->getSettings().format), 0, 0, 0);
		//}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
		if (m_rendererID) {
			glDeleteFramebuffers(1, &m_rendererID);
		}
		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		for (uint32_t i = 0; i < m_colorAttachments.size(); i++) {
			if (!m_colorAttachments[i]) {
				continue;
			}
			m_colorAttachments[i] = Texture2D::create(m_specs.width, m_specs.height, m_colorAttachments[i]->getSettings());
			GLenum texId = NativeRenderObjectCast<GLenum>(m_colorAttachments[i]->getNativeRendererObject());
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texId, 0);
		}

		if (m_depthAttachment) {
			m_depthAttachment = Texture2D::create(m_specs.width, m_specs.height, m_depthAttachment->getSettings());
			GLenum texId = NativeRenderObjectCast<GLenum>(m_depthAttachment->getNativeRendererObject());
			glFramebufferTexture2D(GL_FRAMEBUFFER, getDepthAttachmentTypeByFormat(m_depthAttachment->getSettings().format), GL_TEXTURE_2D, texId, 0);
		}

		updateDrawBuffers();

		CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void OpenGLFramebuffer::resize(uint32_t width, uint32_t height) {
		m_specs.width = width;
		m_specs.height = height;
		invalidate();
	}
	
	void OpenGLFramebuffer::attachDepthTexture(const Ref<Texture2D>& depthText, uint32_t level) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		m_depthAttachment.reset();
		m_depthAttachment = depthText;

		GLenum texId = NativeRenderObjectCast<GLenum>(m_depthAttachment->getNativeRendererObject());
		glFramebufferTexture2D(GL_FRAMEBUFFER, getDepthAttachmentTypeByFormat(m_depthAttachment->getSettings().format), GL_TEXTURE_2D, texId, level);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFramebuffer::detachDepthTexture() {
		if (!m_depthAttachment)
			return;

		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		glFramebufferTexture2D(GL_FRAMEBUFFER, getDepthAttachmentTypeByFormat(m_depthAttachment->getSettings().format), 0, 0, 0);
		m_depthAttachment.reset();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void OpenGLFramebuffer::attachColorTexture(const Ref<Texture2D>& colorText, uint32_t level) {
		attachColorTextureAt((uint32_t)m_colorAttachments.size(), colorText, level);
	}
	void OpenGLFramebuffer::attachColorTextureAt(uint32_t attachment, const Ref<Texture2D>& colorText, uint32_t level) {
		if (attachment > m_colorAttachments.size()) {
			CORE_ASSERT(false, "Attachment index out of range!");
			return;
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		GLenum texId = NativeRenderObjectCast<GLenum>(colorText->getNativeRendererObject());
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, texId, level);

		if(attachment == m_colorAttachments.size())
			m_colorAttachments.push_back(colorText);
		else {
			m_colorAttachments[attachment] = colorText;
		}

		updateDrawBuffers();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFramebuffer::detachColorTexture(uint32_t attachment) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, 0, 0, 0);

		m_colorAttachments[attachment].reset();
		if (m_colorAttachments.size() == attachment + 1) {
			m_colorAttachments.resize(m_colorAttachments.size() - 1);
		}
		updateDrawBuffers();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void OpenGLFramebuffer::updateDrawBuffers() {
		const size_t count = m_colorAttachments.size();
		if (count == 1) {
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
		}
		else if (count > 1) {
			std::vector<GLenum> buffer;
			buffer.reserve(count);

			for (uint32_t i = 0; i < count; i++) {
				GLenum id = NativeRenderObjectCast<GLenum>(m_colorAttachments[i]->getNativeRendererObject());
				buffer[i] = id;
			}

			glDrawBuffers((GLsizei)count, buffer.data());
		}
		else {
			glDrawBuffer(GL_NONE);
		}
	}
}