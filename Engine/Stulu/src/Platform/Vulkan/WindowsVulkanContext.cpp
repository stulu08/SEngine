#include "st_pch.h"
#include "WindowsVulkanContext.h"

#include "Stulu/Core/Application.h"
#include "Stulu/Renderer/GraphicsContext.h"

#include "VulkanFramebuffer.h"
#include "VulkanImage2D.h"

namespace Stulu {
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
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
	}
	WindowsVulkanContext::~WindowsVulkanContext() {
		vkDeviceWaitIdle(device.device);

		vkDestroySemaphore(device.device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device.device, imageAvailableSemaphore, nullptr);
		vkDestroyFence(device.device, inFlightFence, nullptr);

		vkDestroyCommandPool(device.device, device.commandPool, nullptr);

		for (size_t i = 0; i < device.swapChainFrameBuffers.size(); i++) {
			device.swapChainFrameBuffers[i].reset();
		}

		vkDestroyRenderPass(device.device, device.renderPass, nullptr);

		for (size_t i = 0; i < device.swapChainFrameBuffers.size(); i++) {
			device.swapChainImages[i].reset();
		}

		vkDestroySwapchainKHR(device.device, device.swapChain, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyDevice(device.device, nullptr);
		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroyInstance(instance, nullptr);
		glfwTerminate();
	}
	void WindowsVulkanContext::init(Window* window) {
		{
			GLFWwindow* windowHandle = static_cast<GLFWwindow*>(window->getNativeWindow());
			CORE_ASSERT(windowHandle, "Window handle is null");
			m_windowHandle = windowHandle;
		}
		VulkanRenderAPI::setDevice(&device);

		createVulkanInstance();
		setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		getPhysicalDeviceProps();
		createSwapChain();
		createRenderPass();
		createFrameBuffers();
		createCommandBuffer();
		createSemaphores();


		glfwSetFramebufferSizeCallback(m_windowHandle, [](GLFWwindow* window, int width, int height) {
			((WindowsVulkanContext*)Application::get().getWindow().getContext().get())->requestResize();
		});
	}

	void WindowsVulkanContext::beginBuffer() {
		ST_PROFILING_SCOPE("Renderer - Begin Buffers");
		// wait for fences
		vkWaitForFences(device.device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(device.device, 1, &inFlightFence);

		// Acquire an image from the swap chain
		VkResult result = vkAcquireNextImageKHR(device.device, device.swapChain, UINT64_MAX, imageAvailableSemaphore, NULL, &device.imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			resized = false;
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			CORE_ASSERT(false, "failed vkAcquireNextImageKHR!");
		}

		// reset command buffer and begin new
		vkResetCommandBuffer(device.getCommandBuffer(), 0);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(device.getCommandBuffer(), &beginInfo);

		// begin render pass
		device.getFrameBuffer()->bind();
	}

	void WindowsVulkanContext::swapBuffers() {
		ST_PROFILING_SCOPE("Renderer - Swap Buffers");
		// end render pass
		device.getFrameBuffer()->unbind();
		// end command buffer
		vkEndCommandBuffer(device.getCommandBuffer());

		// Submit rendering work
		{
			VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
			VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &device.getCommandBuffer();

			// Wait for the image to become available before rendering
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;

			// Signal when rendering is complete
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
				CORE_ASSERT(false, "failed vkQueueSubmit!");
				return;
			}
		}
		

		// Present the image
		{
			VkSemaphore waitSemaphores[] = { renderFinishedSemaphore };

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &device.swapChain;
			presentInfo.pImageIndices = &device.imageIndex;
			// Wait until rendering is finished before presenting
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = waitSemaphores;

			VkResult result = vkQueuePresentKHR(device.presentQueue, &presentInfo);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || resized) {
				resized = false;
				recreateSwapChain();
			}
			else if (result != VK_SUCCESS) {
				CORE_ASSERT(false, "failed vkQueuePresentKHR!");
			}
		}
		

	}

	void WindowsVulkanContext::setVSync(bool enabled) {
		vkDestroySwapchainKHR(device.device, device.swapChain, nullptr);

		if (enabled) {
			preferedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		}
		else {
			preferedPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		}

		createSwapChain();
	}

	void WindowsVulkanContext::createVulkanInstance() {

		if (!getVulkanVersion()) {
			CORE_ASSERT(false, "Can't get Vulkan version");
		}
		CORE_INFO("Vulkan Version: {0}", vulkanVersion);

		if (enableValidationLayers && !checkValidationLayerSupport()) {
			CORE_ASSERT(false, "Validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = applicationInfo.Name.c_str();
		appInfo.applicationVersion = ST_MAKE_VK_VERSION(applicationInfo.Version);
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
		if (glfwCreateWindowSurface(instance, m_windowHandle, NULL, &surface)) {
			CORE_ASSERT(false, "Could not create Vulkan Surface");
		}
	}
	void WindowsVulkanContext::createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(device.physicalDevice);

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

		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(device.physicalDevice, &createInfo, nullptr, &device.device) != VK_SUCCESS) {
			CORE_ASSERT(false, "failed to create logical device!");
		}

		vkGetDeviceQueue(device.device, indices.graphicsFamily.value(), 0, &device.graphicsQueue);
		vkGetDeviceQueue(device.device, indices.presentFamily.value(), 0, &device.presentQueue);
	}
	void WindowsVulkanContext::pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			CORE_ASSERT(false, "failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& _device : devices) {
			if (isDeviceSuitable(_device)) {
				device.physicalDevice = _device;
				break;
			}
		}

		if (device.physicalDevice == VK_NULL_HANDLE) {
			CORE_ASSERT(false, "failed to find a suitable GPU!");
		}
	}
	void WindowsVulkanContext::createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device.physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = findQueueFamilies(device.physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device.device, &createInfo, nullptr, &device.swapChain) != VK_SUCCESS) {
			CORE_ASSERT(false,"failed to create swap chain!");
		}

		std::vector<VkImage> images;

		vkGetSwapchainImagesKHR(device.device, device.swapChain, &imageCount, nullptr);
		images.resize(imageCount);
		vkGetSwapchainImagesKHR(device.device, device.swapChain, &imageCount, images.data());

		device.swapChainImageFormat = surfaceFormat.format;
		device.swapChainExtent = extent;

		// create image views
		device.swapChainImages.resize(imageCount);
		for (int i = 0; i < device.swapChainImages.size(); i++) {
			VkImageView imageView = {};

			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = images[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = device.swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.layerCount = 1;
			CORE_ASSERT(vkCreateImageView(device.device, &createInfo, nullptr, &imageView) == VK_SUCCESS, "failed to create Image views!");

			device.swapChainImages[i] = createRef<VulkanImage2D>(imageView, device.swapChainExtent.width, device.swapChainExtent.height);
		}
		
	}
	void WindowsVulkanContext::createCommandBuffer() {
		device.commandBuffers.resize(device.swapChainImages.size());
		QueueFamilyIndices indices = findQueueFamilies(device.physicalDevice);

		VkCommandPoolCreateInfo poolCreateInfo = {};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		if (vkCreateCommandPool(device.device, &poolCreateInfo, NULL, &device.commandPool) != VK_SUCCESS) {
			CORE_ASSERT(false, "failed to create command pool!");
		}

		VkCommandBufferAllocateInfo bufferAllocateInfo = {};
		bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		bufferAllocateInfo.commandPool = device.commandPool;
		bufferAllocateInfo.commandBufferCount = (uint32_t)device.commandBuffers.size();
		bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		if (vkAllocateCommandBuffers(device.device, &bufferAllocateInfo, &device.commandBuffers[0]) != VK_SUCCESS) {
			CORE_ASSERT(false, "failed to create command buffers!");
		}

	}
	void WindowsVulkanContext::createSemaphores() {
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(device.device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS) {
			CORE_ASSERT(false, "failed to create semaphore!");
		}
		if(vkCreateSemaphore(device.device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS){
			CORE_ASSERT(false, "failed to create semaphore!");
		}

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		if (vkCreateFence(device.device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
			CORE_ASSERT(false, "failed to create fence!");
		}
	}
	void WindowsVulkanContext::createRenderPass() {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = device.swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		if (vkCreateRenderPass(device.device, &renderPassInfo, nullptr, &device.renderPass) != VK_SUCCESS) {
			CORE_ASSERT(false, "failed to create render pass");
		}
	}
	void WindowsVulkanContext::createFrameBuffers() {
		device.swapChainFrameBuffers.resize(device.swapChainImages.size());

		for (size_t i = 0; i < device.swapChainImages.size(); i++) {


			FrameBufferSpecs specs;
			specs.width = device.swapChainExtent.width;
			specs.height = device.swapChainExtent.height;
			specs.swapChainTarget = true;

			device.swapChainFrameBuffers[i] = createScope<VulkanFramebuffer>(specs, device.swapChainImages[i]);
			/*
			ImageView attachments[] = {
				device.swapChainImageViews[i]
			};
			 
			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = device.renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = device.swapChainExtent.width;
			framebufferInfo.height = device.swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(device.device, &framebufferInfo, nullptr, &device.swapChainFrameBuffers[i]) != VK_SUCCESS) {
				CORE_ASSERT(false, "failed to create framebuffer!");
			}
			*/
		}
	}

	struct PiplelineInfo {
		VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
		VkCullModeFlagBits cullMode = VK_CULL_MODE_BACK_BIT;
		VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;
	};

	void WindowsVulkanContext::cleanupSwapChain() {
		for (size_t i = 0; i < device.swapChainFrameBuffers.size(); i++) {
			device.swapChainFrameBuffers[i].reset();
		}
		for (size_t i = 0; i < device.swapChainFrameBuffers.size(); i++) {
			device.swapChainImages[i].reset();
		}
		vkDestroySwapchainKHR(device.device, device.swapChain, nullptr);

	}

	void WindowsVulkanContext::recreateSwapChain() {
		int width = 0, height = 0;
		glfwGetFramebufferSize(m_windowHandle, &width, &height);
		if (width == 0 || height == 0) {
			return;
		}

		vkDeviceWaitIdle(device.device);

		cleanupSwapChain();

		createSwapChain();
		createFrameBuffers();
	}

	VkSurfaceFormatKHR WindowsVulkanContext::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availFormats) {
		for (const auto& availFormat : availFormats) {
			if(availFormat.format == VK_FORMAT_R8G8B8A8_SRGB || availFormat.format == VK_FORMAT_B8G8R8A8_SRGB || availFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availFormat;
		}
		return availFormats[0];
	}
	VkPresentModeKHR WindowsVulkanContext::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availModes) {
		for (const auto& availMode : availModes) {
			if (availMode == preferedPresentMode)
				return availMode;
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	VkExtent2D WindowsVulkanContext::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(m_windowHandle, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	bool WindowsVulkanContext::isDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = findQueueFamilies(device);
		bool extensionsSupport = checkDeviceExtensionSupport(device);
		bool swapChainAdequate = false;
		if (extensionsSupport) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}
		return indices.isComplete() && extensionsSupport && swapChainAdequate;
	}
	
	bool WindowsVulkanContext::checkValidationLayerSupport() {
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
	bool WindowsVulkanContext::checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	SwapChainSupportDetails WindowsVulkanContext::querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	QueueFamilyIndices WindowsVulkanContext::findQueueFamilies(VkPhysicalDevice device) {
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

	std::vector<const char*> WindowsVulkanContext::getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	bool WindowsVulkanContext::getVulkanVersion() {
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
		vkGetPhysicalDeviceProperties(device.physicalDevice, &device.physicalDeviceProps);
		const char* vendor;
		switch (device.physicalDeviceProps.vendorID)
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
		switch (device.physicalDeviceProps.deviceType)
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
			deviceType = "Unknown Device Type";
			break;
		}
		CORE_INFO("Physical Device Data({3}):\nVendor: {0}\nDevice Name: {1}\nVersion: {2}",
			vendor, device.physicalDeviceProps.deviceName, ST_GET_ST_VERSION(device.physicalDeviceProps.driverVersion), deviceType)
	}
	
	void WindowsVulkanContext::setupDebugMessenger() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			CORE_ASSERT(false,"failed to set up debug messenger!");
		}
	}
	void WindowsVulkanContext::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
#if ST_GRAPHICS_API_VERBOSE_LOGGING
		createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
#endif
#if ST_GRAPHICS_API_INFO_LOGGING
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
#endif
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}
	API_Infos WindowsVulkanContext::getApiInfos() {
		const char* vendor;
		switch (device.physicalDeviceProps.vendorID)
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
		//name, vendor, version, device
		return API_Infos{"Vulkan",vendor,
			vulkanVersion.to_string() + " [" + vendor + " " + ST_GET_ST_VERSION(device.physicalDeviceProps.driverVersion).to_string() + "]",device.physicalDeviceProps.deviceName};
	}
}