#pragma once
#include "Stulu/Renderer/FrameBuffer.h"

#include "VulkanImage2D.h"

namespace Stulu {
	class STULU_API VulkanFramebuffer : public FrameBuffer {
	public:
		VulkanFramebuffer(const FrameBufferSpecs& specs);
		VulkanFramebuffer(const FrameBufferSpecs& specs, const Ref<VulkanImage2D>& image);
		VulkanFramebuffer(const FrameBufferSpecs& specs, const TextureSettings& defaultCB, const TextureSettings& defaultDB);
		virtual ~VulkanFramebuffer();

		virtual void bind(const Viewport& viewport) const override;
		virtual void bind() const override {
			bind({ 0,0,m_specs.width, m_specs.height });
		}

		virtual void unbind() const override;

		virtual void invalidate() override;
		virtual void resize(uint32_t width, uint32_t height, MSAASamples samples) override;
		virtual void resize(uint32_t width, uint32_t height) override {
			resize(width, height, m_specs.samples);
		}

		virtual void attachDepthTexture(const TextureSettings& depthText) override;
		virtual void attachColorTexture(const TextureSettings& colorText) override;
		virtual void attachColorTextureAt(uint32_t attachment, const TextureSettings& colorText) override;

		virtual void detachDepthTexture() override;
		virtual void detachColorTexture(uint32_t attachment) override;

		virtual const FrameBufferSpecs& getSpecs() const override { return m_specs; };
		virtual const Ref<Texture2D>& getColorAttachment(uint32_t index) const override { return m_colorAttachments[0]; }
		virtual const Ref<Texture2D>& getDepthAttachment() const override { return m_depthAttachment; }
		virtual const std::vector<Ref<Texture2D>> getColorAttachments() const override { return m_colorAttachments; }

		virtual void BlitToOther(const Ref<FrameBuffer>& other, bool BlibColor = true, bool BlibDepth = true, bool BlibStencil = true) {
		};

		virtual void SetDrawBuffers(const std::vector<uint32_t>& buffers) override;
		virtual void SetDrawBuffer(uint32_t singleBuffer = 0) override {
			SetDrawBuffers({ singleBuffer });
		}
		virtual void SetReadBuffer(uint32_t singleBuffer = 0) override {

		}

		inline bool isSwapChainTarget() const { return m_specs.swapChainTarget; }
	private:
		VkFramebuffer m_framebuffer = nullptr;
		// use getRenderPass instead
		VkRenderPass m_renderPass = nullptr;

		std::vector<Ref<Texture2D>> m_colorAttachments;
		Ref<Texture2D> m_depthAttachment;

		FrameBufferSpecs m_specs;
	private:
		void cleanUp();
		void createRenderPass();
		void createFrameBuffer();
		VkRenderPass getRenderPass() const;
		VkRenderPass& getRenderPass();
	};

}