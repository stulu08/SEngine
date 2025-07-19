#pragma once
#include "Stulu/Renderer/Texture.h"

namespace Stulu {
	struct FrameBufferSpecs {
		uint32_t width;
		uint32_t height;
		MSAASamples samples;
		bool swapChainTarget;

		FrameBufferSpecs(uint32_t width = 1, uint32_t height = 1, MSAASamples samples = MSAASamples::Disabled, bool swapChainTarget = false)
			: width(width), height(height), samples(samples), swapChainTarget(swapChainTarget)
		{}
	};

	struct Viewport {
		uint32_t x, y;
		uint32_t width, height;
	};

	class STULU_API FrameBuffer {
	public:
		virtual ~FrameBuffer() = default;
		virtual void bind() const = 0;
		virtual void bind(const Viewport& viewport) const = 0;
		virtual void unbind() const = 0;
		virtual void invalidate() = 0;
		virtual void resize(uint32_t width, uint32_t height) = 0;
		virtual void resize(uint32_t width, uint32_t height, MSAASamples samples) = 0;

		virtual void attachDepthTexture(const TextureSettings& depthText) = 0;
		virtual void attachColorTexture(const TextureSettings& colorText) = 0;
		virtual void attachColorTextureAt(uint32_t attachment, const TextureSettings& colorText) = 0;

		virtual void detachDepthTexture() = 0;
		virtual void detachColorTexture(uint32_t attachment = 0) = 0;

		// List of attachment indices, eg: { 0, 2, 3 }
		virtual void SetDrawBuffers(const std::vector<uint32_t>& buffers) = 0;
		// Every framebuffer defaults to using the first color buffer as a draw buffer
		virtual void SetDrawBuffer(uint32_t singleBuffer = 0) = 0;
		virtual void SetReadBuffer(uint32_t singleBuffer = 0) = 0;

		virtual void BlitToOther(const Ref<FrameBuffer>& other, bool BlibColor = true, bool BlibDepth = true, bool BlibStencil = true) = 0;

		virtual const FrameBufferSpecs& getSpecs() const = 0;
		virtual const Ref<Texture2D>& getColorAttachment(uint32_t index = 0) const = 0;
		virtual const Ref<Texture2D>& getDepthAttachment() const = 0;
		virtual const std::vector<Ref<Texture2D>> getColorAttachments() const = 0;

		static Ref<FrameBuffer> create(const FrameBufferSpecs& frameBufferdata, 
			const TextureSettings& defaultColorBuffer = TextureSettings(TextureFormat::RGBA16F, TextureWrap::ClampToEdge),
			const TextureSettings& defaultDepthBuffer = TextureSettings(TextureFormat::Depth24_Stencil8, TextureWrap::ClampToEdge));
		static Ref<FrameBuffer> createEmpty(const FrameBufferSpecs& frameBufferdata);
	};


}