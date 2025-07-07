#pragma once
#include "Stulu/Renderer/FrameBuffer.h"
#include "OpenGLTexture.h"

namespace Stulu {
	class STULU_API OpenGLFramebuffer : public FrameBuffer {
	public:
		OpenGLFramebuffer(const FrameBufferSpecs& specs);
		OpenGLFramebuffer(const FrameBufferSpecs& specs, const TextureSettings& defaultCB, const TextureSettings& defaultDB);
		virtual ~OpenGLFramebuffer();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void invalidate() override;
		virtual void resize(uint32_t width, uint32_t height) override {
			resize(width, height, m_specs.samples);
		}
		virtual void resize(uint32_t width, uint32_t height, MSAASamples samples) override;

		virtual void attachDepthTexture(const TextureSettings& depthText) override;
		virtual void attachColorTexture(const TextureSettings& colorText) override;
		virtual void attachColorTextureAt(uint32_t attachment, const TextureSettings& colorText) override;

		virtual void detachDepthTexture() override;
		virtual void detachColorTexture(uint32_t attachment) override;

		virtual const FrameBufferSpecs& getSpecs() const override { return m_specs; };
		virtual const Ref<Texture2D>& getColorAttachment(uint32_t index = 0) const override { return m_colorAttachments[index]; }
		virtual const Ref<Texture2D>& getDepthAttachment() const override { return m_depthAttachment; }
		virtual const std::vector<Ref<Texture2D>> getColorAttachments() const override { return m_colorAttachments; }

		virtual void SetDrawBuffers(const std::vector<uint32_t>& buffers) override;
		virtual void SetDrawBuffer(uint32_t singleBuffer = 0) override;
		virtual void SetReadBuffer(uint32_t singleBuffer = 0) override;

		virtual void BlitToOther(const Ref<FrameBuffer>& other, bool BlibColor = true, bool BlibDepth = true, bool BlibStencil = true) override;
	private:
		std::vector<Ref<Texture2D>> m_colorAttachments;
		Ref<Texture2D> m_depthAttachment;

		FrameBufferSpecs m_specs;
		uint32_t m_rendererID = 0;

		// calls glFramebufferTexture2D
		void AttachToFrameBuffer(uint32_t slot, const Ref<Texture2D>& texture);

		Ref<OpenGLTexture2D> CreateTexture(const TextureSettings& bufferSettings);
	};

 }