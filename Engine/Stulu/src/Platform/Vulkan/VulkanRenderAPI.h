#pragma once
#include "Stulu/Renderer/RenderAPI.h"
#include "Stulu/Renderer/RenderCommand.h"

#include "VulkanFramebuffer.h"
#include "VulkanShader.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Stulu {
	struct Device {
		Device() {};
		Device(Device&&) = default;

		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;

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
		virtual void StencilNotEqual(uint8_t val, uint8_t ref) override;
		virtual void SetStencilValue(uint8_t value) override;
		virtual void StencilAlways(uint8_t value, uint8_t ref) override;
		virtual void setCullMode(CullMode v) override;
		virtual void clear() override;
		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t indicesCount = 0, const uint32_t instanceCount = 0);
		virtual void drawIndexedSubMesh(const Ref<VertexArray>& vertexArray, const uint32_t indicesCount, const uint32_t indexOffset, const uint32_t vertexOffset = 0, const uint32_t instanceCount = 0);
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

		inline static VulkanRenderAPI* Get() {
			return (VulkanRenderAPI*)RenderCommand::GetApi();
		}

		void CreateGraphicsPipeline(VkPipeline& pipeline, VkPipelineLayout& pipelinelayout, const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages) const;
	private:
		static Device* device;

		VkClearValue clearColor = {};
		VkClearValue clearDepth = {};
	};
}