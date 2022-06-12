#include "st_pch.h"
#include "VulkanShader.h"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

#include "Stulu/Core/Platform.h"

namespace Stulu {
	static VkShaderStageFlagBits shaderTypeFromString(const std::string& type) {
		if (type == "vertex" || type == "vert")
			return VK_SHADER_STAGE_VERTEX_BIT;
		else if (type == "fragment" || type == "frag" || type == "pixel")
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		CORE_ASSERT(false, "Unknown Shadertype: {0}", type);
		return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
	}
	static std::string stringFromShaderType(VkShaderStageFlagBits type) {
		switch (type) {
		case VK_SHADER_STAGE_VERTEX_BIT:
			return "vert";
			break;
		case VK_SHADER_STAGE_FRAGMENT_BIT:
			return "frag";
			break;
		}
		
		CORE_ASSERT(false, "Unknown Shadertype: {0}", type);
		return 0;
	}


	VulkanShader::VulkanShader(const std::string& path) {
		ST_PROFILING_FUNCTION();
		size_t lastS = path.find_last_of("/\\");
		lastS = lastS == std::string::npos ? 0 : lastS + 1;
		size_t lastD = path.rfind('.');
		m_name = path.substr(lastS, lastD == std::string::npos ? path.size() - lastS : lastD - lastS);

		std::string sShaderFile = readFile(path);
		auto sources = preProcess(sShaderFile);
		compile(compileToSpirv(sources, path));


	}
	VulkanShader::VulkanShader(const std::string& name, const std::string& src) {
		ST_PROFILING_FUNCTION();
		m_name = name;
		std::string sShaderFile = readFile(src);
		auto sources = preProcess(sShaderFile);
		compile(compileToSpirv(sources, std::string("unnamed-shaders/") + name + ".glsl"));
	}
	VulkanShader::VulkanShader(const std::string& name, const std::string& vertex, const std::string& fragment)
		: m_name(name) {
		ST_PROFILING_FUNCTION();
		std::string src = "##type " + stringFromShaderType(VK_SHADER_STAGE_VERTEX_BIT) + "\n" + vertex + "\n" + "##type " + stringFromShaderType(VK_SHADER_STAGE_FRAGMENT_BIT) + "\n" + fragment + "\n";
		auto sources = preProcess(src);
		compile(compileToSpirv(sources, std::string("unnamed-shaders/") + name + ".glsl"));
	}

	void VulkanShader::createFile(const std::string& path, const std::string& content) {
		std::fstream stream(path,std::ios::out);
		if (stream.is_open()) {
			stream << content;
		}
		else {
			CORE_ASSERT(false,"Cant write to file " + path);
		}
		stream.close();
	}

	std::string VulkanShader::readFile(const std::string& path) {
		ST_PROFILING_FUNCTION();
		std::string result;
		std::ifstream inStream(path, std::ios::in | std::ios::binary);

		if (inStream) {
			inStream.seekg(0, std::ios::end);
			result.resize(inStream.tellg());
			inStream.seekg(0, std::ios::beg);
			inStream.read(&result[0], result.size());
			inStream.close();
		}
		else
			CORE_ERROR("Could not open File: {0}", path);

		return result;
	}
	std::string VulkanShader::readSpirvFile(const std::string& path) {
		ST_PROFILING_FUNCTION();
		std::string result;
		std::ifstream inStream(path, std::ios::ate | std::ios::binary);

		if (inStream) {
			inStream.seekg(0, std::ios::end);
			result.resize(inStream.tellg());
			inStream.seekg(0, std::ios::beg);
			inStream.read(&result[0], result.size());
			inStream.close();
		}
		else
			CORE_ERROR("Could not open File: {0}", path);

		return result;
	}
	std::unordered_map<VkShaderStageFlagBits, std::string> VulkanShader::preProcess(const std::string& _src) {
		ST_PROFILING_FUNCTION();
		std::unordered_map<VkShaderStageFlagBits, std::string> shaderSources;
		std::string shaderSrc = _src;
		{//##add
			//everyone can only be added once 
			for (auto& p : s_preProcessorAdds) {
				std::string token = "##add " + p.first;
				size_t pos = shaderSrc.find(token);
				if (pos != std::string::npos) {
					shaderSrc.replace(pos, token.length(), p.second);
				}
			}
		}
		{//##properity
			std::istringstream stream{ shaderSrc };
			std::string line;
			const std::string token = "##properity";
			while (std::getline(stream, line)) {
				size_t pos = line.find(token);
				if (pos != std::string::npos) {
					line.replace(pos, token.length() + 1, "");//remove
					size_t firstBracket = line.find_first_of('(');
					size_t lastBracket = line.find_first_of(')');
					std::string values = line.substr(firstBracket + 1, lastBracket - firstBracket - 1);
					ShaderProperity::Type pType = ShaderProperity::typeFromString(line.substr(0, firstBracket));

					std::string typeName = line.substr(line.find_first_of('|') + 1);
					typeName.erase(std::remove(typeName.begin(), typeName.end(), '\r'), typeName.end());
					typeName.erase(std::remove(typeName.begin(), typeName.end(), '\n'), typeName.end());

					if (pType == ShaderProperity::Type::Color4) {
						m_properitys[typeName] = createRef<ShaderProperityColor4>();
					}
					else if (pType == ShaderProperity::Type::Range) {
						m_properitys[typeName] = createRef<ShaderProperityRange>(values);
					}
					else if (pType == ShaderProperity::Type::Enum) {
						m_properitys[typeName] = createRef<ShaderProperityEnum>(values);
					}

				}
			}
		}
		{//##type
			const char* typeToken = "##type";
			size_t typeTokenLength = strlen(typeToken);
			size_t pos = shaderSrc.find(typeToken, 0);
			while (pos != std::string::npos)
			{
				size_t eol = shaderSrc.find_first_of("\r\n", pos);
				CORE_ASSERT(eol != std::string::npos, "Syntax error");
				size_t begin = pos + typeTokenLength + 1;
				std::string type = shaderSrc.substr(begin, eol - begin);
				CORE_ASSERT(shaderTypeFromString(type), "Invalid shader type specified");

				size_t nextLinePos = shaderSrc.find_first_not_of("\r\n", eol);
				CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
				pos = shaderSrc.find(typeToken, nextLinePos);
				shaderSources[shaderTypeFromString(type)] = (pos == std::string::npos) ? shaderSrc.substr(nextLinePos) : shaderSrc.substr(nextLinePos, pos - nextLinePos);
			}
		}
		m_source = _src;
		m_sourcePreProcessed = shaderSrc;
		return shaderSources;
	}
	std::unordered_map<VkShaderStageFlagBits, std::string> VulkanShader::compileToSpirv(const std::unordered_map<VkShaderStageFlagBits, std::string>& shaderSrcs, std::string shaderPath) {
		std::unordered_map<VkShaderStageFlagBits, std::string> shaderSources;
		size_t lastS = shaderPath.find_last_of("/\\");
		std::string shaderPathDirectory = shaderPath.substr(0, lastS);;
		for (auto i : shaderSrcs) {
			std::string uncompiledPath = shaderPathDirectory + "/" + m_name + "." + stringFromShaderType(i.first);
			std::string compiledPath = uncompiledPath + ".spv";
			createFile(uncompiledPath, i.second);

			std::string exec = ("glslc.exe " + uncompiledPath + " -o " + compiledPath);
			system(exec.c_str());
			
			shaderSources[i.first] = readSpirvFile(compiledPath);

			std::remove(uncompiledPath.c_str());
			std::remove(compiledPath.c_str());
		}
		return shaderSources;
	}
	VkShaderModule VulkanShader::createShaderModule(const std::string& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(VulkanRenderAPI::getDevice().device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			CORE_ASSERT(false,"failed to create shader module!");
		}

		return shaderModule;
	}
	void VulkanShader::compile(const std::unordered_map<VkShaderStageFlagBits, std::string>& shaderSrcs) {
		ST_PROFILING_FUNCTION();
		CORE_ASSERT(shaderSrcs.size() <= 2, "Only 2 shaders are currently supported");
		int32_t shaderIndex = 0;
		std::array<VkShaderModule, 2> shaderModules;
		for (auto& kv : shaderSrcs) {
			VkShaderStageFlagBits stage = kv.first;
			const std::string& src = kv.second;
			VkShaderModule shader = createShaderModule(src);

			VkPipelineShaderStageCreateInfo shaderStageInfo{};
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.stage = stage;
			shaderStageInfo.module = shader;
			shaderStageInfo.pName = "main";
			m_shaderStages[shaderIndex] = shaderStageInfo;
			shaderModules[shaderIndex] = shader;
			shaderIndex++;
		}

		for (auto shader : shaderModules) {
			vkDestroyShaderModule(VulkanRenderAPI::getDevice().device, shader, nullptr);
		}
	}
	VulkanShader::~VulkanShader() {
		ST_PROFILING_FUNCTION();
		
	}

	void VulkanShader::reload(const std::string& path) {
		std::string sShaderFile = readFile(path);
		auto sources = preProcess(sShaderFile);
		compile(compileToSpirv(sources, path));
	}

	void VulkanShader::bind() const {
		ST_PROFILING_FUNCTION();
		
	}
	void VulkanShader::unbind() const {
		ST_PROFILING_FUNCTION();
	}
	void VulkanShader::setMat4(const std::string& name, const glm::mat4& mat) {
		uploadMat4Uniform(name, mat);
	}
	void VulkanShader::setFloat4(const std::string& name, const glm::vec4& vec) {
		uploadFloat4Uniform(name, vec);
	}
	void VulkanShader::setFloat3(const std::string& name, const glm::vec3& vec) {
		uploadFloat3Uniform(name, vec);
	}
	void VulkanShader::setFloat2(const std::string& name, const glm::vec2& vec) {
		uploadFloat2Uniform(name, vec);
	}
	void VulkanShader::setFloat(const std::string& name, const float Float) {
		uploadFloatUniform(name, Float);
	}
	void VulkanShader::setInt(const std::string& name, const int32_t Int) {
		uploadIntUniform(name, Int);
	}
	void VulkanShader::setIntArray(const std::string& name, const int* values, uint32_t count) {
		uploadIntArrayUniform(name, values, count);
	}
	void VulkanShader::uploadMat4Uniform(const std::string& name, const glm::mat4& matrix) {
		ST_PROFILING_FUNCTION();
		
	}
	void VulkanShader::uploadMat3Uniform(const std::string& name, const glm::mat3& matrix) {
		ST_PROFILING_FUNCTION();

	}
	void VulkanShader::uploadFloat4Uniform(const std::string& name, const glm::vec4& float4) {
		ST_PROFILING_FUNCTION();

	}
	void VulkanShader::uploadFloat3Uniform(const std::string& name, const glm::vec3& float3) {
		ST_PROFILING_FUNCTION();

	}
	void VulkanShader::uploadFloat2Uniform(const std::string& name, const glm::vec2& float2) {
		ST_PROFILING_FUNCTION();

	}
	void VulkanShader::uploadIntUniform(const std::string& name, const int32_t _int) {
		ST_PROFILING_FUNCTION();

	}
	void VulkanShader::uploadIntArrayUniform(const std::string& name, const int* values, uint32_t count) {
		ST_PROFILING_FUNCTION();

	}
	void VulkanShader::uploadFloatUniform(const std::string& name, const float _float) {
		ST_PROFILING_FUNCTION();

	}
}
