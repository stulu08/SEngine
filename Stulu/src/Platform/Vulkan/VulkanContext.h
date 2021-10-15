#pragma once
#include "Stulu/Renderer/GraphicsContext.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct GLFWwindow;
namespace Stulu {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
	class VulkanContext : public GraphicsContext {
	public:
		VulkanContext(GLFWwindow* windowHandle);
		virtual ~VulkanContext();

		virtual void init() override;
		virtual void swapBuffers() override;

	private:
#ifndef ST_DEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkSurfaceKHR surface;
		GLFWwindow* m_windowHandle;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};



		void createVulkanInstance();
		void createSurface();
		void pickPhysicalDevice();
		void setupDebugMessenger();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		bool checkValidationLayerSupport();
		void createLogicalDevice();
		bool isDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		std::vector<const char*> getRequiredExtensions();
	};
}


