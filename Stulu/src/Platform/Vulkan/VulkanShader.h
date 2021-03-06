#pragma once
#include "Stulu/Renderer/Shader.h"
#include "VulkanRenderAPI.h"

namespace Stulu {
	class STULU_API VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& path);
		VulkanShader(const std::string& name, const std::string& src);
		VulkanShader(const std::string& name, const std::string& vertex, const std::string& fragment);
		virtual ~VulkanShader();

		virtual void bind() const override;
		virtual void unbind() const override;
		virtual const std::string& getName() const override { return m_name; }
		virtual void setMat4(const std::string& name, const glm::mat4& mat) override;
		virtual void setFloat4(const std::string& name, const glm::vec4& vec) override;
		virtual void setFloat3(const std::string& name, const glm::vec3& vec) override;
		virtual void setFloat2(const std::string& name, const glm::vec2& vec) override;
		virtual void setFloat(const std::string& name, const float Float) override;
		virtual void setInt(const std::string& name, const int32_t Int) override;
		virtual void setIntArray(const std::string& name, const int* values, uint32_t count) override;
		virtual void uploadMat4Uniform(const std::string& name, const glm::mat4& matrix);
		virtual void uploadMat3Uniform(const std::string& name, const glm::mat3& matrix);
		virtual void uploadFloat4Uniform(const std::string& name, const glm::vec4& float4);
		virtual void uploadFloat3Uniform(const std::string& name, const glm::vec3& float3);
		virtual void uploadFloat2Uniform(const std::string& name, const glm::vec2& float2);
		virtual void uploadFloatUniform(const std::string& name, const float _float);
		virtual void uploadIntUniform(const std::string& name, const int32_t _int);
		virtual void uploadIntArrayUniform(const std::string& name, const int* values, uint32_t count);

	private:
		VkPipelineShaderStageCreateInfo m_shaderStages[2];
		std::string m_name;

		void createFile(const std::string& path, const std::string& content);
		std::string readFile(const std::string& path);
		std::string readSpirvFile(const std::string& path);
		std::unordered_map<VkShaderStageFlagBits, std::string> preProcess(const std::string src, bool multiFile = false);
		std::unordered_map<VkShaderStageFlagBits, std::string> compileToSpirv(const std::unordered_map<VkShaderStageFlagBits, std::string>& shaderSrcs, std::string shaderPath);
		VkShaderModule createShaderModule(const std::string& code);
		void compile(const std::unordered_map<VkShaderStageFlagBits, std::string>& shaderSrcs);
		bool isMultiFile(const std::string src);
	};
}

