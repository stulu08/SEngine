#pragma once
#include "Stulu/Renderer/FrameBuffer.h"

namespace Stulu {
	class STULU_API OpenGLFrameBufferTexture : public FrameBufferTexture {
	public:
		OpenGLFrameBufferTexture(uint32_t width, uint32_t height, uint32_t samples = 0);
		virtual ~OpenGLFrameBufferTexture();

		virtual void invalidate(TextureSettings::Format& format) override;
		virtual void resize(uint32_t width, uint32_t height) override;

		virtual void* getColorAttachmentRendereObject() const override { return (void*)&m_colorAttachment; };
		virtual void* getDepthAttachmentRendereObject() const override { return (void*)&m_depthAttachment; };
		virtual void* getNativeRendererObject() const override { return (void*)&m_colorAttachment; };
		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }
		virtual TextureSettings& getSettings() override { return m_settings; }

		virtual void bind(uint32_t slot = 0) const override;
		virtual void bindDepthAttachment(uint32_t slot = 0) const override;
		virtual bool operator == (const Texture& other) const override { return m_colorAttachment == *static_cast<uint32_t*>(other.getNativeRendererObject()); };
		virtual operator int() override { return m_colorAttachment; }
	private:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_colorAttachment = 0;
		uint32_t m_depthAttachment = 0;
		uint32_t m_samples = 0;
		TextureSettings m_settings;
	};

	class OpenGLFramebuffer : public FrameBuffer{
	public:
		OpenGLFramebuffer(const FrameBufferSpecs& frameBufferdata);
		virtual ~OpenGLFramebuffer();

		virtual void bind() const override;
		virtual void unbind() const override;
		virtual void invalidate() override;
		virtual void resize(uint32_t width, uint32_t height) override;

		virtual void attachCubeMapFace(const Ref<CubeMap>& cubemap, uint32_t face) override;

		virtual FrameBufferSpecs& getSpecs() override;
		virtual Ref<FrameBufferTexture>& getTexture() override { return m_texture; }


	private:
		FrameBufferSpecs m_specs;
		uint32_t m_rendererID = 0;
		Ref<FrameBufferTexture> m_texture;
	};

 }