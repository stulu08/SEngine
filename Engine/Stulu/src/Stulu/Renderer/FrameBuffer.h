#pragma once
#include "Stulu/Renderer/Texture.h"

namespace Stulu {
	struct FrameBufferSpecs {
		uint32_t width;
		uint32_t height;
		uint32_t samples;
		bool swapChainTarget;

		FrameBufferSpecs(uint32_t width = 1, uint32_t height = 1, uint32_t samples = 1, bool swapChainTarget = false)
			: width(width), height(height), samples(samples), swapChainTarget(swapChainTarget)
		{}
	};
	class STULU_API FrameBuffer {
	public:
		virtual ~FrameBuffer() = default;
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void invalidate() = 0;
		virtual void resize(uint32_t width, uint32_t height) = 0;

		virtual void attachDepthTexture(const Ref<Texture2D>& depthText, uint32_t level = 0) = 0;
		virtual void attachColorTexture(const Ref<Texture2D>& colorText, uint32_t level = 0) = 0;
		virtual void attachColorTextureAt(uint32_t attachment, const Ref<Texture2D>& colorText, uint32_t level = 0) = 0;

		virtual void detachDepthTexture() = 0;
		virtual void detachColorTexture(uint32_t attachment = 0) = 0;

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