#include "st_pch.h"
#include "VulkanRenderAPI.h"

namespace Stulu {
	Device* VulkanRenderAPI::device = nullptr;

	void VulkanRenderAPI::init() {
		
	}
	void VulkanRenderAPI::setDefault() {
		
	}
	void VulkanRenderAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {

	}
	void VulkanRenderAPI::setWireFrame(bool v ){

	}
	void VulkanRenderAPI::setClearColor(const glm::vec4& color) {
		clearColor.color =  *(VkClearColorValue*)glm::value_ptr(color);
		clearDepth.depthStencil = { 1.0f, 0 }; 
	}
	void VulkanRenderAPI::setDepthTesting(bool value) {

	}
	void VulkanRenderAPI::setStencil(StencilMode v) {

	}
	void VulkanRenderAPI::setCullMode(CullMode v) {

	}
	void VulkanRenderAPI::clear() {
		std::array<VkClearAttachment, 2> attachments = { };
		attachments[0] = { VK_IMAGE_ASPECT_COLOR_BIT, 0, clearColor.color };
		attachments[1] = { VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, clearDepth.color };

		VkClearRect rect = {  };
		rect.rect = { {0, 0}, device->swapChainExtent };
		rect.layerCount = 1;
		rect.baseArrayLayer = 0;

		vkCmdClearAttachments(device->getCommandBuffer(), (uint32_t)attachments.size(), attachments.data(), 1, &rect);
	}
	void VulkanRenderAPI::drawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t count, const uint32_t instanceCount) {

	}
	void VulkanRenderAPI::drawLines(const Ref<VertexArray>& vertexArray, const uint32_t count) {

	}
}