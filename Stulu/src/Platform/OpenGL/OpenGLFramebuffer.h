#pragma once
#include "Stulu/Renderer/FrameBuffer.h"

namespace Stulu {
	class STULU_API OpenGLFrameBufferTexture : public FrameBufferTexture {
	public:
		OpenGLFrameBufferTexture(uint32_t width, uint32_t height);
		virtual ~OpenGLFrameBufferTexture();

		virtual void invalidate(TextureSettings::Format& format) override;
		virtual void resize(uint32_t width, uint32_t height) override;

		virtual uint32_t getColorAttachmentRendereID() const override { return m_colorAttachment; };
		virtual uint32_t getDepthAttachmentRendereID() const override { return m_depthAttachment; };
		virtual uint32_t getRendererID() const override { return m_colorAttachment; };
		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }

		virtual void bind(uint32_t slot = 0) const override;
		virtual bool operator == (const Texture& other) const override { return getRendererID() == other.getRendererID(); };
		virtual operator int() override { return m_colorAttachment; }
	private:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_colorAttachment = 0;
		uint32_t m_depthAttachment = 0;
	};

	class OpenGLFramebuffer : public FrameBuffer{
	public:
		OpenGLFramebuffer(const FrameBufferSpecs& frameBufferdata);
		virtual ~OpenGLFramebuffer();

		virtual void bind() const override;
		virtual void unbind() const override;
		virtual void invalidate() override;
		virtual void resize(uint32_t width, uint32_t height) override;

		virtual FrameBufferSpecs& getSpecs() override;
		virtual Ref<FrameBufferTexture>& getTexture() override { return m_texture; }


	private:
		FrameBufferSpecs m_specs;
		uint32_t m_rendererID = 0;
		Ref<FrameBufferTexture> m_texture;
	};

 }