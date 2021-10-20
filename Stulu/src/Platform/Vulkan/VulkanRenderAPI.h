#pragma once
#include "Stulu/Renderer/RenderAPI.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
namespace Stulu {

	struct Device {
		VkDevice device;
		VkPhysicalDeviceProperties physicalDeviceProps;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkSwapchainKHR swapChain;
		VkExtent2D swapChainExtent;
		VkFormat swapChainImageFormat;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
	};
	class VulkanRenderAPI : public RenderAPI {
	public:
		virtual void init() override;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void setDepthFunc(bool lequalLess) override;
		virtual void setDepthMask(bool value) override;
		virtual void setWireFrame(bool v) override;
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;
		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t count = 0) override;



		inline static Device& getDevice() {
			return *s_device;
		}
		inline static void setDevice(Device* device) {
			s_device = device;
		}
	private:
		inline static Device* s_device = nullptr;
	};
}