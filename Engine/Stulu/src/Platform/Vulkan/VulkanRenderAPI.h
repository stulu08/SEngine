#pragma once
#include "Stulu/Renderer/RenderAPI.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
namespace Stulu {
	class STULU_API VulkanFramebuffer;
	class STULU_API VulkanImage2D;

	struct Device {
		VkDevice device;

		VkPhysicalDeviceProperties physicalDeviceProps;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		VkQueue graphicsQueue;
		VkQueue presentQueue;
		
		VkSwapchainKHR swapChain;
		VkExtent2D swapChainExtent;
		VkFormat swapChainImageFormat;
		std::vector<Ref<VulkanImage2D>> swapChainImages;
		std::vector<Scope<VulkanFramebuffer>> swapChainFrameBuffers;

		std::vector<VkCommandBuffer> commandBuffers;
		VkCommandPool commandPool;

		VkRenderPass renderPass;

		uint32_t imageIndex;

		VkRenderPass lastRenderPass = nullptr;

		VkCommandBuffer& getCommandBuffer() {
			return commandBuffers[imageIndex];
		}
		Ref<VulkanImage2D>& getImage() {
			return swapChainImages[imageIndex];
		}
		Scope<VulkanFramebuffer>& getFrameBuffer() {
			return swapChainFrameBuffers[imageIndex];
		}
	};
	class STULU_API VulkanRenderAPI : public RenderAPI {
	public:
		virtual void init() override;
		virtual void setDefault() override;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void setWireFrame(bool v) override;
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void setDepthTesting(bool value) override;
		virtual void setStencil(StencilMode v) override;
		virtual void setCullMode(CullMode v) override;
		virtual void clear() override;
		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t count = 0, const uint32_t instanceCount = 0) override;
		virtual void drawLines(const Ref<VertexArray>& vertexArray, const uint32_t count = 0) override;

		virtual const glm::ivec3 getMaxComputeWorkGroupCount() const override { return { 0,0,0 }; }
		virtual const glm::ivec3 getMaxComputeWorkGroupSizes() const override{ return { 0,0,0 }; }
		virtual const uint32_t getMaxComputeWorkGroupInvocationCount() const override{ return 0; }

		inline static Device& getDevice() {
			return *device;
		}
		inline static void setDevice(Device* vkDevice) {
			device = vkDevice;
		}

	private:
		static Device* device;

		VkClearValue clearColor = {};
		VkClearValue clearDepth = {};
	};
}