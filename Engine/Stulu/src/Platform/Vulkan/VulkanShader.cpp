#include "st_pch.h"
#include "VulkanShader.h"
#include "VulkanRenderAPI.h"

namespace Stulu {
    static VkShaderStageFlagBits shaderTypeToVk(ShaderType type) {
        switch (type) {
        case ShaderType::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case ShaderType::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        case ShaderType::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
            break;
        }
        CORE_ERROR("Unknown Shadertype: {0}", type);
        return VK_SHADER_STAGE_VERTEX_BIT;
    }

    VulkanShader::VulkanShader(const std::string& name, const ShaderCompileResult& sources) {
        m_name = name;
        link(sources);
    }
    VulkanShader::~VulkanShader() {
        cleanup();
    }
    void VulkanShader::reload(const ShaderCompileResult& sources) {
        cleanup();
        link(sources);
    }

    void VulkanShader::bind() const {
        auto& device = VulkanRenderAPI::getDevice();
        vkCmdBindPipeline(device.getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeLine);
    }

    void VulkanShader::unbind() const {

    }

    void VulkanShader::Dispatch(const glm::uvec3& numWorkGroups, uint32_t usage) {
        
    }

    void VulkanShader::cleanup() {
        auto& device = VulkanRenderAPI::getDevice();

        vkDestroyPipeline(device.device, m_pipeLine, nullptr);
        vkDestroyPipelineLayout(device.device, m_pipelineLayout, nullptr);
    }

    void VulkanShader::link(const ShaderCompileResult& sources) {
        std::vector<VkPipelineShaderStageCreateInfo> stages;
        std::vector<VkShaderModule> modules;

        stages.resize(sources.Size());
        modules.resize(sources.Size());

        for (int i = 0; i < sources.Size(); i++) {
            auto& [type, compileResult] = sources.Get(i);

            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = compileResult.data.size() * sizeof(compileResult.data[0]);
            createInfo.pCode = &compileResult.data[0];

            VkShaderModule& shaderModule = modules[i];
            if (vkCreateShaderModule(VulkanRenderAPI::getDevice().device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                CORE_ASSERT(false, "Failed to create shader module");
            }

            VkPipelineShaderStageCreateInfo& vertShaderStageInfo = stages[i];
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = shaderTypeToVk(type);
            vertShaderStageInfo.module = shaderModule;
            vertShaderStageInfo.pName = "main";

        }

        VulkanRenderAPI::Get()->CreateGraphicsPipeline(m_pipeLine, m_pipelineLayout, stages);

        for (auto module : modules) {
            vkDestroyShaderModule(VulkanRenderAPI::getDevice().device, module, nullptr);
        }

	}
}