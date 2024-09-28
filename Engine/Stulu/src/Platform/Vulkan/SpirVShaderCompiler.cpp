#include "st_pch.h"
#include "SpirvShaderCompiler.h"

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <glslang/Public/ResourceLimits.h>

#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
	inline EShLanguage ShaderTypeToLang(ShaderType type) {
		switch (type)
		{
		case ShaderType::Vertex:
			return EShLanguage::EShLangVertex;
		case ShaderType::Fragment:
			return EShLanguage::EShLangFragment;
		case ShaderType::Compute:
			return EShLanguage::EShLangCompute;
		}
		return EShLanguage::EShLangVertex;
	}

	bool SpirVShaderCompiler::s_glslangInitlized = false;

	SpirVShaderCompiler::SpirVShaderCompiler() {
		if (!s_glslangInitlized) {
			glslang::InitializeProcess();
			s_glslangInitlized = true;
		}
	}
	constexpr bool combined_shader_modules = true;
	const std::string version_string = R"(
#version 450
//#extension GL_ARB_separate_shader_objects : enable
)";

	ShaderCompileResult SpirVShaderCompiler::Compile(const ShaderSource& sources) const {
		EShMessages messages = EShMsgDebugInfo;
		glslang::TProgram program;
		std::vector<std::pair<ShaderType, Scope<glslang::TShader>>> shaders;
		std::string log;

		for (int i = 0; i < sources.Size(); i++) {
			auto& [stage, source] = sources.Get(i);

			shaders.push_back({ stage, createScope<glslang::TShader>(ShaderTypeToLang(stage)) });
			glslang::TShader* shader = shaders.back().second.get();

			std::string glslSource = (version_string + source);
			std::array<const char*, 1> shadersSource = { glslSource.c_str() };
			
			if (Renderer::getRendererAPI() == Renderer::API::OpenGL) {
				shader->setEnvClient(glslang::EShClient::EShClientOpenGL, glslang::EShTargetOpenGL_450);
			}

			if (Renderer::getRendererAPI() == Renderer::API::Vulkan) {
				shader->setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetVulkan_1_3);
				messages = EShMsgVulkanRules;
			}
			
			shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_6);
			shader->setStrings(shadersSource.data(), 1);
			shader->setEntryPoint("main");
			shader->setSourceEntryPoint("main");

			if (!shader->parse(GetDefaultResources(), 450, true, messages)) {
				CORE_ERROR("Shader spirv parsing failed: {0}", shader->getInfoLog());
				CORE_ASSERT(false, "Spirv failed");
				return {};
			}
			log = shader->getInfoDebugLog();
			if (!log.empty())
				CORE_INFO(log);

			if constexpr (combined_shader_modules)
				program.addShader(shader);
		}

		if constexpr (combined_shader_modules) {
			if (!program.link(messages)) {
				CORE_ERROR("Shader spirv linking failed: {0}", program.getInfoLog());
				CORE_ASSERT(false, "Spirv failed");
				return {};
			}
			log = program.getInfoDebugLog();
			if (!log.empty())
				CORE_INFO(log);
		}


		ShaderCompileResult result;

		for (auto& [stage, shader] : shaders) {
			glslang::TIntermediate* intermediate;

			if constexpr (combined_shader_modules) {
				intermediate = program.getIntermediate(ShaderTypeToLang(stage));
			}
			else {
				intermediate = shader->getIntermediate();
			}

			std::vector<uint32_t> dat;
			glslang::GlslangToSpv(*intermediate, dat);
			result.Add(stage, CompiledShaderData{ dat });
		}


		return result;
	}

	ShaderCompileResult SpirVShaderCompiler::CompileToCache(const ShaderSource& sources, const std::string& cacheFile) const {
		ShaderCompileResult result = Compile(sources);

		std::filesystem::path path = cacheFile;

		if (path.has_parent_path() && !std::filesystem::exists(path.parent_path()))
			std::filesystem::create_directories(path.parent_path());


		for (int i = 0; i < result.Size(); i++) {
			auto& [type, res] = result.Get(i);

			const std::string name = path.string() + "." + std::to_string(type);

			FILE* file = fopen(name.c_str(), "wb");

			//fwrite(&type, sizeof(type), 1, file);
			fwrite(&res.data[0], sizeof(res.data[0]), res.data.size(), file);

			fclose(file);
		}

		return result;
	}

	ShaderCompileResult SpirVShaderCompiler::LoadFromCache(const std::string& cacheFile) const {
		return {};
	}

	bool SpirVShaderCompiler::isCacheUpToDate(const std::string& cacheFile, const std::string& shaderSourceFile) const {
		struct stat cacheResult;
		struct stat shaderResult;

		if (stat(cacheFile.c_str(), &cacheResult) != 0) {
			return false;
		}
		if (stat(shaderSourceFile.c_str(), &shaderResult) != 0) {
			return false;
		}

		return false;
	}
}