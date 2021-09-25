#pragma once
#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
	struct FrameBufferSpecs {
		uint32_t width, height;
		uint32_t samples = 1;
		bool swapChainTarget = false;
	};

	class FrameBuffer {
	public:
		virtual ~FrameBuffer() = default;
		virtual void bind() const = 0;
		virtual void unBind() const = 0;
		virtual void Invalidate() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		
		virtual FrameBufferSpecs& getSpecs() = 0;
		virtual uint32_t getColorAttachmentRendereID() const = 0;
		virtual uint32_t getDepthAttachmentRendereID() const = 0;

		static Ref<FrameBuffer> create(const FrameBufferSpecs& frameBufferdata);
	};
}