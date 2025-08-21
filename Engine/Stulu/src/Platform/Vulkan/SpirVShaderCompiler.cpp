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
		case ShaderType::Geometry:
			return EShLanguage::EShLangGeometry;
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

	bool SpirVShaderCompiler::Compile(const ShaderSource& sources, ShaderCompileResult& result) const {
		EShMessages messages = EShMsgDebugInfo;
		glslang::TProgram program;
		std::vector<std::pair<ShaderType, Scope<glslang::TShader>>> shaders;
		std::string log;
		
		for (int i = 0; i < sources.Size(); i++) {
			const auto& [stage, source] = sources.Get(i);
			auto shaderStage = ShaderTypeToLang(stage);

			shaders.push_back({ stage, createScope<glslang::TShader>(shaderStage) });
			glslang::TShader* shader = shaders.back().second.get();

			std::string glslSource = ApplyHeaders(source);
			std::array<const char*, 1> shadersSource = { glslSource.c_str() };
			shader->setStrings(shadersSource.data(), 1);

			if (Renderer::getRendererAPI() == Renderer::API::OpenGL) {
				shader->setEnvInput(glslang::EShSourceGlsl, shaderStage, glslang::EShClientOpenGL, 450);
				shader->setEnvClient(glslang::EShClient::EShClientOpenGL, glslang::EShTargetOpenGL_450);
			}
			
			if (Renderer::getRendererAPI() == Renderer::API::Vulkan) {
				shader->setEnvInput(glslang::EShSourceGlsl, shaderStage, glslang::EShClientVulkan, 100);
				shader->setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetVulkan_1_3);
				messages = EShMsgVulkanRules;
			}
			
			shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);

			shader->setEntryPoint("main");
			shader->setSourceEntryPoint("main");

			if (!shader->parse(GetDefaultResources(), 450, true, messages)) {
				CORE_ERROR("SpirV Compilation ERROR(s) for {0}-Shader:\n{1}", std::to_string(stage), shader->getInfoLog());
				return false;
			}
			log = shader->getInfoDebugLog();
			if (!log.empty())
				CORE_INFO(log);

			if constexpr (combined_shader_modules)
				program.addShader(shader);
		}

		if constexpr (combined_shader_modules) {
			if (!program.link(messages)) {
				CORE_ERROR("SpirV Linking ERROR(s):\n{0}", program.getInfoLog());
				return false;
			}
			log = program.getInfoDebugLog();
			if (!log.empty())
				CORE_INFO(log);
		}

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

			shader.reset();
		}


		return true;
	}

	bool SpirVShaderCompiler::CompileToCache(const ShaderSource& sources, const std::string& cacheFile, ShaderCompileResult& result) const {
		if (Compile(sources, result)) {
			std::filesystem::path path = cacheFile;

			if (path.has_parent_path() && !std::filesystem::exists(path.parent_path()))
				std::filesystem::create_directories(path.parent_path());


			for (int i = 0; i < result.Size(); i++) {
				const auto& [type, res] = result.Get(i);

				const std::string name = path.string() + "." + std::to_string(type);

				FILE* file = fopen(name.c_str(), "wb");

				//fwrite(&type, sizeof(type), 1, file);
				fwrite(&res.data[0], sizeof(res.data[0]), res.data.size(), file);

				fclose(file);
			}
			return true;
		}
		return false;
	}

	bool SpirVShaderCompiler::LoadFromCache(const std::string& cacheFile, ShaderCompileResult& result) const {
		return false;
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
	std::string SpirVShaderCompiler::ApplyHeaders(const std::string& src) const {
		std::stringstream stream;
		for (const auto& header : m_headers) {
			stream << header << std::endl;
		}
		stream << "/* Stulu SpirV Shader Compiler Headers End */" << std::endl;
		stream << src;
		return stream.str();
	}
}