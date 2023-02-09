#pragma once
#include "Stulu/Renderer/FrameBuffer.h"
#include "OpenGLTexture.h"

namespace Stulu {
	class STULU_API OpenGLFrameBufferTexture : public FrameBufferTexture {
	public:
		OpenGLFrameBufferTexture(FrameBufferSpecs& specs)  
			: m_specs(specs) {}
		virtual ~OpenGLFrameBufferTexture();

		virtual void invalidate() override;
		virtual void resize(FrameBufferSpecs& specs) override;

		virtual void* getColorAttachmentRendereObject() const override { return (void*)&m_colorAttachment; };
		virtual Ref<Texture2D> getColorAttachment() const override { return m_colorTexture; };
		virtual void* getDepthAttachmentRendereObject() const override { return (void*)&m_depthAttachment; };
		virtual Ref<Texture2D> getDepthAttachment() const override { return m_depthTexture; };
		virtual void* getNativeRendererObject() const override { return (void*)&m_colorAttachment; };
		virtual uint32_t getWidth() const override { return m_specs.width; }
		virtual uint32_t getHeight() const override { return  m_specs.height; }
		virtual TextureSettings& getSettings() override { return m_specs.colorTexture; }
		virtual TextureSettings& getDepthSettings() override { return m_specs.depthTexture; }
		virtual TextureSettings& getColorSettings() override { return m_specs.colorTexture; }
		virtual FrameBufferSpecs& getSpecs()  override { return m_specs; };


		virtual void bind(uint32_t slot = 0) const override;
		virtual void bindDepthAttachment(uint32_t slot = 0) const override;
		virtual bool operator == (const Texture& other) const override { return m_colorAttachment == *static_cast<uint32_t*>(other.getNativeRendererObject()); };
		virtual operator int() override { return m_colorAttachment; }
	private:
		void updateTextures();

		uint32_t m_colorAttachment = 0;
		uint32_t m_depthAttachment = 0;
		FrameBufferSpecs m_specs;

		Ref<OpenGLTexture2D> m_depthTexture = nullptr, m_colorTexture = nullptr;
	};

	class OpenGLFramebuffer : public FrameBuffer{
	public:
		OpenGLFramebuffer(const FrameBufferSpecs& frameBufferdata);
		virtual ~OpenGLFramebuffer();

		virtual void bind() const override;
		virtual void unbind() const override;
		virtual void invalidate() override;
		virtual void resize(uint32_t width, uint32_t height) override;

		virtual void attachCubeMapFace(const Ref<CubeMap>& cubemap, uint32_t face, uint32_t mip) override;

		virtual FrameBufferSpecs& getSpecs() override { return m_specs; };
		virtual Ref<FrameBufferTexture>& getTexture() override { return m_texture; }


	private:
		FrameBufferSpecs m_specs;
		uint32_t m_rendererID = 0;
		Ref<FrameBufferTexture> m_texture;
	};

 }