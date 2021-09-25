#pragma once
#include "Stulu/Renderer/FrameBuffer.h"

namespace Stulu {
	class OpenGLFramebuffer : public FrameBuffer{
	public:
		OpenGLFramebuffer(const FrameBufferSpecs& frameBufferdata);
		virtual ~OpenGLFramebuffer();

		virtual void bind() const override;
		virtual void unBind() const override;
		virtual void Invalidate() override;
		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual FrameBufferSpecs& getSpecs() override;
		virtual uint32_t getColorAttachmentRendereID() const override { return m_colorAttachment; }
		virtual uint32_t getDepthAttachmentRendereID() const override { return m_depthAttachment; }


	private:
		FrameBufferSpecs m_specs;
		uint32_t m_rendererID = 0;
		uint32_t m_colorAttachment = 0;
		uint32_t m_depthAttachment = 0;
	};
 }