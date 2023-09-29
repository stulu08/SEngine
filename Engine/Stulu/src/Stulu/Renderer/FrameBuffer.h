#pragma once
#include "Stulu/Renderer/Texture.h"

namespace Stulu {
	
	struct FrameBufferSpecs {
		uint32_t width = 1, height = 1;
		uint32_t samples = 1;
		bool swapChainTarget = false;

		TextureSettings colorTexture = TextureSettings(TextureFormat::RGBA16F, TextureWrap::ClampToEdge);
		TextureSettings depthTexture = TextureSettings(TextureFormat::Depth24_Stencil8, TextureWrap::ClampToEdge);

		FrameBufferSpecs(uint32_t width = 1, uint32_t height = 1, uint32_t samples = 1, bool swapChainTarget = false,
			TextureSettings colorTexture = TextureSettings(TextureFormat::RGBA16F, TextureWrap::ClampToEdge),
			TextureSettings depthTexture = TextureSettings(TextureFormat::Depth24_Stencil8, TextureWrap::ClampToEdge))
			: width(width), height(height), samples(samples), swapChainTarget(swapChainTarget), colorTexture(colorTexture), depthTexture(depthTexture)
		{}
	};
	class STULU_API FrameBufferTexture : public Texture{
	public:
		static Ref<FrameBufferTexture> create(FrameBufferSpecs& specs);

		virtual void invalidate() = 0;
		virtual void resize(FrameBufferSpecs& specs) = 0;
		

		virtual void* getColorAttachmentRendereObject() const = 0;
		virtual Ref<Texture2D> getColorAttachment() const = 0;
		virtual void* getDepthAttachmentRendereObject() const = 0;
		virtual Ref<Texture2D> getDepthAttachment() const = 0;
		virtual TextureSettings& getDepthSettings() = 0;
		virtual TextureSettings& getColorSettings() = 0;

		virtual void bindDepthAttachment(uint32_t slot) const = 0;

		virtual FrameBufferSpecs& getSpecs() = 0;
	};
	class STULU_API FrameBuffer {
	public:
		virtual ~FrameBuffer() = default;
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void invalidate() = 0;
		virtual void resize(uint32_t width, uint32_t height) = 0;

		virtual void attachCubeMapFace(const Ref<CubeMap>& cubemap, uint32_t face, uint32_t mip = 0) = 0;

		virtual FrameBufferSpecs& getSpecs() = 0;
		virtual Ref<FrameBufferTexture>& getTexture() = 0;

		static Ref<FrameBuffer> create(const FrameBufferSpecs& frameBufferdata);
	};


}