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
	void VulkanRenderAPI::StencilNotEqual(uint8_t val, uint8_t ref){
	
	}
	void VulkanRenderAPI::StencilAlways(uint8_t val, uint8_t ref){
	
	}
	void VulkanRenderAPI::SetStencilValue(uint8_t value){
	
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
	void VulkanRenderAPI::drawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t indicesCount, const uint32_t instanceCount) {
	}

	void VulkanRenderAPI::drawIndexedSubMesh(const Ref<VertexArray>& vertexArray, const uint32_t indicesCount, const uint32_t indexOffset, const uint32_t vertexOffset, const uint32_t instanceCount) {
		
	}
	void VulkanRenderAPI::drawLines(const Ref<VertexArray>& vertexArray, const uint32_t count)
	{
	}
	void VulkanRenderAPI::CreateGraphicsPipeline(VkPipeline& pipeline, VkPipelineLayout& pipelineLayout, const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages) const {
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout(device->device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			CORE_ASSERT(false, "failed to create graphics pipeline layout");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = (uint32_t)shaderStages.size();
		pipelineInfo.pStages = &shaderStages[0];
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = device->renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(device->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
			CORE_ASSERT(false, "failed to create graphics pipeline");
		}
	}
}