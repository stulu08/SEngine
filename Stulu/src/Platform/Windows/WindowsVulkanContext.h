#pragma once
#include "Stulu/Renderer/GraphicsContext.h"
#include "Stulu/Core/Application.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#define ST_MAKE_VK_VERSION(version) VK_MAKE_VERSION(version.major, version.minor, version.patch)
#define ST_GET_ST_VERSION(uintVersion) Version(VK_VERSION_MAJOR(uintVersion),VK_VERSION_MINOR(uintVersion),VK_VERSION_PATCH(uintVersion))

struct GLFWwindow;
namespace Stulu {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	class WindowsVulkanContext : public GraphicsContext {
	public:
		WindowsVulkanContext();
		virtual ~WindowsVulkanContext();

		virtual void init(Window* window) override;
		virtual void swapBuffers() override;
		virtual void setVSync(bool enabled) override;
	private:
#ifndef ST_DEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif
		GLFWwindow* m_windowHandle;

		VkDevice device;
		VkInstance instance;
		VkSurfaceKHR surface;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDeviceProperties physicalDeviceProps;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkSwapchainKHR swapChain;
		VkExtent2D swapChainExtent;
		VkFormat swapChainImageFormat;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

		//Mailbox = no Vsync
		//Fifo = Vsync
		VkPresentModeKHR preferedPresentMode = VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR;

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		Version vulkanVersion;
		ApplicationInfo applicationInfo;

		void createVulkanInstance();
		void createSurface();
		void createLogicalDevice();
		void pickPhysicalDevice();
		void createSwapChain();
		void createImageViews();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		bool isDeviceSuitable(VkPhysicalDevice device);

		bool checkValidationLayerSupport();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);

		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

		std::vector<const char*> getRequiredExtensions();
		bool getVulkanVersion();
		void getPhysicalDeviceProps();

		void setupDebugMessenger();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	};
}


