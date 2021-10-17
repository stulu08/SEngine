#include "st_pch.h"
#include "WindowsVulkanContext.h"

#include "Stulu/Core/Application.h"
#include "Stulu/Renderer/GraphicsContext.h"
namespace Stulu {
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		ST_PROFILING_FUNCTION();
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		ST_PROFILING_FUNCTION();
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			CORE_TRACE("[Vulkan][ValidationLayer] {0}", pCallbackData->pMessage)
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			CORE_INFO("[Vulkan][ValidationLayer] {0}", pCallbackData->pMessage)
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			CORE_WARN("[Vulkan][ValidationLayer] {0}", pCallbackData->pMessage)
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			CORE_ERROR("[Vulkan][ValidationLayer] {0}", pCallbackData->pMessage)
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			break;
		default:
			break;
		}

		return VK_FALSE;
	}
	
	
	WindowsVulkanContext::WindowsVulkanContext()
		: applicationInfo(Application::get().getApplicationInfo()) {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	}
	WindowsVulkanContext::~WindowsVulkanContext() {
		ST_PROFILING_FUNCTION();
		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyDevice(device, nullptr);
		vkDestroyInstance(instance, nullptr);
		glfwTerminate();
	}
	void WindowsVulkanContext::init(Window* window) {
		ST_PROFILING_FUNCTION();
		{
			GLFWwindow* windowHandle = static_cast<GLFWwindow*>(window->getNativeWindow());
			CORE_ASSERT(windowHandle, "Window handle is null");
			m_windowHandle = windowHandle;
		}
		createVulkanInstance();
		setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		getPhysicalDeviceProps();
	}
	void WindowsVulkanContext::swapBuffers() {
		ST_PROFILING_FUNCTION();
		
	}

	void WindowsVulkanContext::setVSync(bool enabled) {
	}

	void WindowsVulkanContext::createVulkanInstance() {
		ST_PROFILING_FUNCTION();

		if (!getVulkanVersion()) {
			CORE_ASSERT(false, "Can't get Vulkan version");
		}
		CORE_INFO("Vulkan Version: {0}", vulkanVersion);

		if (enableValidationLayers && !checkValidationLayerSupport()) {
			CORE_ASSERT(false, "Validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = applicationInfo.ApplicationName;
		appInfo.applicationVersion = ST_MAKE_VK_VERSION(applicationInfo.ApplicationVersion);
		appInfo.pEngineName = ST_ENGINE_NAME;
		appInfo.engineVersion = ST_MAKE_VK_VERSION(ST_ENGINE_VERSION);
		appInfo.apiVersion = VK_MAKE_VERSION(vulkanVersion.major, vulkanVersion.minor, 0);

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
	
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			CORE_ASSERT(false, "failed to create instance!");
		}
	}
	void WindowsVulkanContext::createSurface() {
		ST_PROFILING_FUNCTION();
		if (glfwCreateWindowSurface(instance, m_windowHandle, NULL, &surface)) {
			CORE_ASSERT(false, "Could not create Vulkan Surface");
		}
	}
	void WindowsVulkanContext::createLogicalDevice() {
		ST_PROFILING_FUNCTION();
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = 0;

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			CORE_ASSERT(false, "failed to create logical device!");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}
	void WindowsVulkanContext::setupDebugMessenger() {
		ST_PROFILING_FUNCTION();
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			CORE_ASSERT(false,"failed to set up debug messenger!");
		}
	}
	void WindowsVulkanContext::pickPhysicalDevice() {
		ST_PROFILING_FUNCTION();
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			CORE_ASSERT(false, "failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE) {
			CORE_ASSERT(false, "failed to find a suitable GPU!");
		}
	}

	void WindowsVulkanContext::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		ST_PROFILING_FUNCTION();
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
#if ST_VULKAN_VERBOSE_LOGGING
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
#elif ST_VULKAN_INFO_LOGGING
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
#else
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
#endif // VULKAN_VERBOSE_LOGGING
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}
	
	bool WindowsVulkanContext::isDeviceSuitable(VkPhysicalDevice device) {
		ST_PROFILING_FUNCTION();
		QueueFamilyIndices indices = findQueueFamilies(device);
		return indices.isComplete();
	}
	
	QueueFamilyIndices WindowsVulkanContext::findQueueFamilies(VkPhysicalDevice device) {
		ST_PROFILING_FUNCTION();
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport) {
				indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}
	bool WindowsVulkanContext::checkValidationLayerSupport() {
		ST_PROFILING_FUNCTION();
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> WindowsVulkanContext::getRequiredExtensions() {
		ST_PROFILING_FUNCTION();
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	typedef VkResult(VKAPI_PTR* _vkEnumerateInstanceVersion)(uint32_t*);
	bool WindowsVulkanContext::getVulkanVersion() {
		ST_PROFILING_FUNCTION();
		auto FN_vkEnumerateInstanceVersion = PFN_vkEnumerateInstanceVersion(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));
		if (FN_vkEnumerateInstanceVersion != nullptr) {
			uint32_t api_version;
			VkResult res = FN_vkEnumerateInstanceVersion(&api_version);
			if (res == VK_SUCCESS) {
				vulkanVersion = ST_GET_ST_VERSION(api_version);
			}
			else {
				CORE_ASSERT(false, "Cant read vulkan version");
				return false;
			}
		}
		else {
			CORE_ASSERT(false,"vkEnumerateInstanceVersion not available, assuming Vulkan 1.0.");
			return false;
		}
		if ((vulkanVersion.major > 1) || (vulkanVersion.major == 1 && vulkanVersion.minor > 2)) {
			vulkanVersion.major = 1;
			vulkanVersion.minor = 2;
			vulkanVersion.patch = 0;
		}
		return true;
	}
	void WindowsVulkanContext::getPhysicalDeviceProps() {
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProps);
		const char* vendor;
		switch (physicalDeviceProps.vendorID)
		{
		case 0x1002:
			vendor = "Advanced Micro Devices, Inc. [AMD/ATI]";
			break;
		case 0x10DE:
			vendor = "NVIDIA Corporation";
			break;
		case 0x8086:
			vendor = "INTEL Corporation";
			break;
		case 0x13B5:
			vendor = "ARM";
			break;
		default:
			vendor = "Unknown";
			break;
		}
		const char* deviceType;
		switch (physicalDeviceProps.deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			deviceType = "GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			deviceType = "Integrated GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			deviceType = "Virtual GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			deviceType = "CPU";
			break;
		default:
			break;
		}
		CORE_INFO("Physical Device Data({3}):\nVendor: {0}\nDevice Name: {1}\nVersion: {2}",
			vendor, physicalDeviceProps.deviceName, ST_GET_ST_VERSION(physicalDeviceProps.driverVersion), deviceType)
	}
}