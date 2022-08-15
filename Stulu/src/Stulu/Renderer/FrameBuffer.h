#pragma once
#include "Stulu/Renderer/Texture.h"

namespace Stulu {
	struct FrameBufferSpecs {
		uint32_t width = 1, height = 1;
		uint32_t samples = 1;
		bool swapChainTarget = false;
		TextureSettings::Format textureFormat = TextureSettings::Format::RGBA16F;
	};
	class STULU_API FrameBufferTexture : public Texture{
	public:
		static Ref<FrameBufferTexture> create(uint32_t width, uint32_t height);

		virtual void invalidate(TextureSettings::Format& format) = 0;
		virtual void resize(uint32_t width, uint32_t height) = 0;
		

		virtual uint32_t getColorAttachmentRendereID() const = 0;
		virtual uint32_t getDepthAttachmentRendereID() const = 0;

		virtual void bindDepthAttachment(uint32_t slot) const = 0;

	};
	class STULU_API FrameBuffer {
	public:
		virtual ~FrameBuffer() = default;
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void invalidate() = 0;
		virtual void resize(uint32_t width, uint32_t height) = 0;

		virtual void attachCubeMapFace(const Ref<CubeMap> cubemap, uint32_t face) = 0;

		virtual FrameBufferSpecs& getSpecs() = 0;
		virtual Ref<FrameBufferTexture>& getTexture() = 0;

		static Ref<FrameBuffer> create(const FrameBufferSpecs& frameBufferdata);
	};


}