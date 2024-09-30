#include "st_pch.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderAPI.h"

namespace Stulu {
	VulkanFramebuffer::VulkanFramebuffer(const FrameBufferSpecs& specs) 
		: m_specs(specs) {

		if (!m_specs.swapChainTarget)
			createRenderPass();

		createFrameBuffer();
	}
	VulkanFramebuffer::VulkanFramebuffer(const FrameBufferSpecs& specs, const Ref<VulkanImage2D>& image)
		: m_specs(specs) {
		m_colorAttachments.push_back(image);

		if (!m_specs.swapChainTarget)
			createRenderPass();

		createFrameBuffer();
	}
	VulkanFramebuffer::VulkanFramebuffer(const FrameBufferSpecs& specs, const TextureSettings& defaultCB, const TextureSettings& defaultDB) {

	}
	VulkanFramebuffer::~VulkanFramebuffer() {
		cleanUp();
	}
	void VulkanFramebuffer::bind() const {
		if (VulkanRenderAPI::getDevice().lastRenderPass != nullptr) {
			CORE_TRACE("Another or same renderpass still active, automaticly ending it");
			vkCmdEndRenderPass(VulkanRenderAPI::getDevice().getCommandBuffer());
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = getRenderPass(); // your created render pass
		renderPassInfo.framebuffer = m_framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = { m_specs.width, m_specs.height };

		std::array<VkClearValue, 1> clearValues{};
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 0.0f} };
		//clearValues[1].depthStencil = { 0.0f, 0 };
		renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(VulkanRenderAPI::getDevice().getCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	
		VulkanRenderAPI::getDevice().lastRenderPass = getRenderPass();
	}
	void VulkanFramebuffer::unbind() const {
		vkCmdEndRenderPass(VulkanRenderAPI::getDevice().getCommandBuffer());
		
		VulkanRenderAPI::getDevice().lastRenderPass = nullptr;
		if (!isSwapChainTarget()) {
			CORE_TRACE("Switching to swapchain render pass");
			VulkanRenderAPI::getDevice().getFrameBuffer()->bind();
		}
	}
	void VulkanFramebuffer::invalidate() {
		cleanUp();

		for (size_t i = 0; i < m_colorAttachments.size(); i++) {
			if (!m_colorAttachments[i]) {
				continue;
			}

			TextureSettings settings = m_colorAttachments[i]->getSettings();
			m_colorAttachments[i] = createRef<VulkanImage2D>(m_specs.width, m_specs.height, settings);
		}

		if (!isSwapChainTarget())
			createRenderPass();
		createFrameBuffer();
	}
	void VulkanFramebuffer::resize(uint32_t width, uint32_t height) {
		m_specs.width = width;
		m_specs.height = height;
		invalidate();
	}
	void VulkanFramebuffer::attachDepthTexture(const Ref<Texture2D>& depthText, uint32_t level)
	{
	}
	void VulkanFramebuffer::attachColorTexture(const Ref<Texture2D>& colorText, uint32_t level) {
		attachColorTextureAt((uint32_t)m_colorAttachments.size(), colorText, level);
	}
	void VulkanFramebuffer::attachColorTextureAt(uint32_t attachment, const Ref<Texture2D>& colorText, uint32_t level) {
		cleanUp();

		if (attachment == m_colorAttachments.size())
			m_colorAttachments.push_back(colorText);
		else {
			m_colorAttachments[attachment] = colorText;
		}

		if (!isSwapChainTarget())
			createRenderPass();
		createFrameBuffer();
	}
	void VulkanFramebuffer::detachDepthTexture()
	{
	}
	void VulkanFramebuffer::detachColorTexture(uint32_t attachment)
	{
	}
	void VulkanFramebuffer::cleanUp() {
		if (m_framebuffer != nullptr) {
			vkDestroyFramebuffer(VulkanRenderAPI::getDevice().device, m_framebuffer, nullptr);
			m_framebuffer = nullptr;
		}
		if (!isSwapChainTarget() && m_renderPass != nullptr) {
			vkDestroyRenderPass(VulkanRenderAPI::getDevice().device, getRenderPass(), nullptr);
			m_renderPass = nullptr;
		}
	}
	void VulkanFramebuffer::createRenderPass() {
		if (isSwapChainTarget())
			return;

		auto& device = VulkanRenderAPI::getDevice();

		if (m_renderPass != nullptr)
			vkDestroyRenderPass(device.device, getRenderPass(), nullptr);

		std::vector<VkAttachmentDescription> colorDescriptions;
		std::vector<VkAttachmentReference> colorRefrences;
		colorDescriptions.resize(m_colorAttachments.size());
		colorRefrences.resize(m_colorAttachments.size());
		
		for (size_t i = 0; i < m_colorAttachments.size(); i++) {
			VkAttachmentDescription& colorAttachment = colorDescriptions[i];
			colorAttachment.format = device.swapChainImageFormat;
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference& colorAttachmentRef = colorRefrences[i];
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = (uint32_t)colorRefrences.size();
		subpass.pColorAttachments = colorRefrences.data();

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = (uint32_t)colorDescriptions.size();;
		renderPassInfo.pAttachments = colorDescriptions.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		if (vkCreateRenderPass(device.device, &renderPassInfo, nullptr, &getRenderPass()) != VK_SUCCESS) {
			CORE_ASSERT(false, "failed to create render pass");
		}
	}
	void VulkanFramebuffer::createFrameBuffer() {
		if (m_framebuffer != nullptr) {
			vkDestroyFramebuffer(VulkanRenderAPI::getDevice().device, m_framebuffer, nullptr);
		}

		std::vector<VkImageView> attachments;
		for (auto& image : m_colorAttachments) {
			attachments.push_back((VkImageView)image->getNativeRendererObject());
		}

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = getRenderPass();
		framebufferInfo.attachmentCount = (uint32_t)attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_specs.width;
		framebufferInfo.height = m_specs.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(VulkanRenderAPI::getDevice().device, &framebufferInfo, nullptr, &m_framebuffer) != VK_SUCCESS) {
			CORE_ASSERT(false, "failed to create framebuffer!");
		}
	}
	inline VkRenderPass VulkanFramebuffer::getRenderPass() const {
		return isSwapChainTarget() ? VulkanRenderAPI::getDevice().renderPass : m_renderPass;
	}
	VkRenderPass& VulkanFramebuffer::getRenderPass() {
		return isSwapChainTarget() ? VulkanRenderAPI::getDevice().renderPass : m_renderPass;
	}
}