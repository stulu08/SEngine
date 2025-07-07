#include "st_pch.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLStateCache.h"
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

	void OpenGLFramebuffer::AttachToFrameBuffer(uint32_t slot, const Ref<Texture2D>& texture) {
		GLenum textureType = std::dynamic_pointer_cast<OpenGLTexture2D>(texture)->GetInternalTextureType();
		GLenum texId = NativeRenderObjectCast<GLenum>(texture->getNativeRendererObject());

		if (slot == GL_DEPTH_ATTACHMENT) {
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				getDepthAttachmentTypeByFormat(texture->getSettings().format),
				textureType, texId, 0);
		}
		else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, slot, textureType, texId, 0);
		}
	}

	Ref<OpenGLTexture2D> OpenGLFramebuffer::CreateTexture(const TextureSettings& bufferSettings) {
		auto texture = Texture2D::create(m_specs.width, m_specs.height, bufferSettings, m_specs.samples);
		return std::dynamic_pointer_cast<OpenGLTexture2D>(texture);
	}


	OpenGLFramebuffer::OpenGLFramebuffer(const FrameBufferSpecs& specs)
		: m_specs(specs) {

		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		SetDrawBuffer();

		CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FrameBufferSpecs& specs, const TextureSettings& colorBuffer, const TextureSettings& depthBuffer)
		: m_specs(specs) {

		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		if (colorBuffer.format != TextureFormat::None) {
			m_colorAttachments.push_back(CreateTexture(colorBuffer));
			AttachToFrameBuffer(GL_COLOR_ATTACHMENT0, m_colorAttachments[0]);
		}
		if (depthBuffer.format != TextureFormat::None) {
			m_depthAttachment = CreateTexture(depthBuffer);
			AttachToFrameBuffer(GL_DEPTH_ATTACHMENT, m_depthAttachment);
		}

		SetDrawBuffer();

		CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGLFramebuffer::~OpenGLFramebuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
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
			m_colorAttachments[i] = CreateTexture(m_colorAttachments[i]->getSettings());
			AttachToFrameBuffer(GL_COLOR_ATTACHMENT0 + i, m_colorAttachments[i]);
		}

		if (m_depthAttachment) {
			m_depthAttachment = CreateTexture(m_depthAttachment->getSettings());
			AttachToFrameBuffer(GL_DEPTH_ATTACHMENT, m_depthAttachment);
		}

		SetDrawBuffer();

		CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void OpenGLFramebuffer::resize(uint32_t width, uint32_t height, MSAASamples samples) {
		m_specs.width = width;
		m_specs.height = height;
		m_specs.samples = samples;
		invalidate();
	}
	
	void OpenGLFramebuffer::attachDepthTexture(const TextureSettings& depthText) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		m_depthAttachment.reset();
		m_depthAttachment = CreateTexture(depthText);
		AttachToFrameBuffer(GL_DEPTH_ATTACHMENT, m_depthAttachment);

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
	
	void OpenGLFramebuffer::attachColorTexture(const TextureSettings& colorText) {
		attachColorTextureAt((uint32_t)m_colorAttachments.size(), colorText);
	}
	void OpenGLFramebuffer::attachColorTextureAt(uint32_t attachment, const TextureSettings& ColorTextureSettings) {
		if (attachment > m_colorAttachments.size()) {
			CORE_ASSERT(false, "Attachment index out of range!");
			return;
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		auto colorText = CreateTexture(ColorTextureSettings);
		AttachToFrameBuffer(GL_COLOR_ATTACHMENT0 + attachment, colorText);

		if(attachment == m_colorAttachments.size())
			m_colorAttachments.push_back(colorText);
		else {
			m_colorAttachments[attachment] = colorText;
		}

		SetDrawBuffer();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFramebuffer::detachColorTexture(uint32_t attachment) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, 0, 0, 0);

		m_colorAttachments[attachment].reset();
		if (m_colorAttachments.size() == attachment + 1) {
			m_colorAttachments.resize(m_colorAttachments.size() - 1);
		}
		SetDrawBuffer();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	

	void OpenGLFramebuffer::SetDrawBuffers(const std::vector<uint32_t>& buffers) {
		const size_t countBuffers = std::min(buffers.size(), m_colorAttachments.size());
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		
		if (countBuffers < 1) {
			glDrawBuffer(GL_NONE);
		}
		else if (countBuffers == 1) {
			glDrawBuffer(GL_COLOR_ATTACHMENT0 + buffers[0]);
		}
		else {
			std::vector<GLenum> glBufferList;
			glBufferList.resize(countBuffers);

			for (uint32_t i = 0; i < countBuffers; i++) {
				glBufferList[i] = GL_COLOR_ATTACHMENT0 + buffers[i];
			}

			glDrawBuffers((GLsizei)countBuffers, glBufferList.data());
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::SetDrawBuffer(uint32_t singleBuffer) {
		SetDrawBuffers({ singleBuffer });
	}
	void OpenGLFramebuffer::SetReadBuffer(uint32_t singleBuffer) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + singleBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFramebuffer::BlitToOther(const Ref<FrameBuffer>& other, bool BlibColor, bool BlibDepth, bool BlibStencil) {
		OpenGLFramebuffer* otherBuffer = (OpenGLFramebuffer*)other.get();

		GLenum filter = 0;
		if (BlibColor)
			filter |= GL_COLOR_BUFFER_BIT;
		if (BlibDepth)
			filter |= GL_DEPTH_BUFFER_BIT;
		if (BlibStencil)
			filter |= GL_STENCIL_BUFFER_BIT;

		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rendererID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, otherBuffer->m_rendererID);
		glBlitFramebuffer(
			0, 0, m_specs.width, m_specs.height, 
			0, 0, otherBuffer->m_specs.width, otherBuffer->m_specs.height, 
			filter, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}