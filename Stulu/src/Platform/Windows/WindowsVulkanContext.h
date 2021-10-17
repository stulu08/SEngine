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

		Version vulkanVersion;

		VkInstance instance;
		VkSurfaceKHR surface;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDeviceProperties physicalDeviceProps;
		GLFWwindow* m_windowHandle;

		VkQueue graphicsQueue;
		VkQueue presentQueue;


		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		const ApplicationInfo applicationInfo;

		void createVulkanInstance();
		void createSurface();
		void createLogicalDevice();
		void setupDebugMessenger();
		void pickPhysicalDevice();

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		bool isDeviceSuitable(VkPhysicalDevice device);

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		bool checkValidationLayerSupport();

		std::vector<const char*> getRequiredExtensions();
		bool getVulkanVersion();
		void getPhysicalDeviceProps();
	};
}


