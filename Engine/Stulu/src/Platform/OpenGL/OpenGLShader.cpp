#include "st_pch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "Stulu/Resources/Resources.h"
#include "Stulu/Renderer/Shader.h"
#include "OpenGLTexture.h"

namespace Stulu {
	
	static GLenum shaderTypeToGl(ShaderType type) {
		switch (type) {
		case ShaderType::Vertex:
			return GL_VERTEX_SHADER;
			break;
		case ShaderType::Fragment:
			return GL_FRAGMENT_SHADER;
			break;
		case ShaderType::Compute:
			return GL_COMPUTE_SHADER;
			break;
		}
		CORE_ERROR("Unknown Shadertype: {0}", type);
		return GL_NONE;
	}

	OpenGLShaderCompiler::OpenGLShaderCompiler() {}

	void OpenGLShaderCompiler::Compile(const ShaderSource& sources, ShaderCompileResult& result) const {
		GLuint rendererID = glCreateProgram();

		std::vector<GLenum> shaderIds;
		shaderIds.resize(sources.Size(), 0);

		for (uint32_t i = 0; i < sources.Size(); i++) {
			const auto& [type, strSrc] = sources.Get(i);

			GLuint shader = glCreateShader(shaderTypeToGl(type));

			const std::string src = ApplyHeaders(strSrc);
			const GLchar* source = src.c_str();

			glShaderSource(shader, 1, &source, 0);
			glCompileShader(shader);
			
			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE) {
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
				glDeleteShader(shader);
				CORE_ERROR("GLSL {1} Shader compilation error:\n{0}", infoLog.data(), std::to_string(type));
				break;
			}

			glAttachShader(rendererID, shader);
			shaderIds[i] = shader;
		}
		glLinkProgram(rendererID);

		GLint isLinked = 0;
		glGetProgramiv(rendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(rendererID, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(rendererID, maxLength, &maxLength, &infoLog[0]);
			glDeleteProgram(rendererID);
			for (auto id : shaderIds)
				glDeleteShader(id);

			CORE_ERROR("GLSL compilation error:\n{0}", infoLog.data());
			CORE_ASSERT(false, "Could not link shader program");
			return;
		}

		for (auto id : shaderIds) {
			glDetachShader(rendererID, id);
			glDeleteShader(id);
		}


		// Retrieve the binary format and binary length
		GLint bufferLength = 0;
		GLenum binaryFormat = 0;
		GLsizei programSize = 0;
		glGetProgramiv(rendererID, GL_PROGRAM_BINARY_LENGTH, &bufferLength);
		
		size_t uint32_count = (bufferLength + sizeof(uint32_t) - 1) / sizeof(uint32_t);
		std::vector<uint32_t> binary(uint32_count + 2);

		unsigned char* binaryData = reinterpret_cast<unsigned char*>(&binary[2]);

		glGetProgramBinary(rendererID, bufferLength, &programSize, &binaryFormat, binaryData);
		CORE_ASSERT(bufferLength == programSize, "Error while retrieving program binary")

		binary[0] = (uint32_t)binaryFormat;
		binary[1] = (uint32_t)bufferLength;

		result.Add(Stulu::ShaderType::Vertex, { std::move(binary) });

		glDeleteProgram(rendererID);
	}
	void OpenGLShaderCompiler::CompileToCache(const ShaderSource& sources, const std::string& cacheFile, ShaderCompileResult& result) const {
		Compile(sources, result);

		std::filesystem::path path = cacheFile;
		if (path.has_parent_path() && !std::filesystem::exists(path.parent_path()))
			std::filesystem::create_directories(path.parent_path());

		if (result.Size() != 1) {
			CORE_ASSERT(false, "Invalid compilation result of OpenGLShaderCompiler");
			return;
		}

		const auto& [type, res] = result.Get(0);
		const std::string name = path.string();
		FILE* file = fopen(name.c_str(), "wb");

		fwrite(&res.data[0], sizeof(res.data[0]), res.data.size(), file);
		fclose(file);

		return;

	}
	void OpenGLShaderCompiler::LoadFromCache(const std::string& cacheFile, ShaderCompileResult& result) const {
	
	}
	bool OpenGLShaderCompiler::isCacheUpToDate(const std::string& cacheFile, const std::string& shaderSourceFile) const {
		return false;
	}

	std::string OpenGLShaderCompiler::ApplyHeaders(const std::string& src) const {
		std::stringstream result;
		
		for (auto& header : m_headers) {
			result << header << "\n";
		}
		result << src;
		return result.str();
	}


	OpenGLShader::OpenGLShader(const std::string& name, const ShaderCompileResult& sources)
		: m_name(name) {
		link(sources);
	}

	OpenGLShader::~OpenGLShader() {
		glDeleteProgram(m_rendererID);
	}

	void OpenGLShader::reload(const ShaderCompileResult& sources) {
		glDeleteProgram(m_rendererID);
		m_rendererID = 0;
		link(sources);
	}

	void OpenGLShader::link(const ShaderCompileResult& sources) {
		GLuint rendererID = glCreateProgram();
		std::vector<GLenum> shaderIds;

		if (!Renderer::getShaderSystem()->SpirvSupported()) {
			// opengl compiled shaders

			if (sources.Size() != 1) {
				CORE_ASSERT(false, "Invalid compilation result of OpenGLShaderCompiler");
				return;
			}

			const auto& result = sources.Get(0).second;

			uint32_t format = result.data[0];  
			uint32_t binSize = result.data[1]; 

			const unsigned char* bin = reinterpret_cast<const unsigned char*>(&result.data[2]);

			glProgramBinary(rendererID, (GLenum)format, bin, (GLsizei)binSize);
		}
		else {
			shaderIds.resize(sources.Size(), 0);

			// SPIRV compiled shaders
			for (uint32_t i = 0; i < sources.Size(); i++) {
				const auto& [type, result] = sources.Get(i);
				GLuint shader = glCreateShader(shaderTypeToGl(type));

				glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, &result.data[0], (GLsizei)(result.data.size() * sizeof(result.data[0])));
				glSpecializeShader(shader, "main", 0, nullptr, nullptr);

				shaderIds[i] = shader;
			}


			for (auto id : shaderIds) {
				glAttachShader(rendererID, id);
			}

			glLinkProgram(rendererID);
		}


		GLint isLinked = 0;
		glGetProgramiv(rendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE) {
			GLint maxLength = 512;
			glGetProgramiv(rendererID, GL_INFO_LOG_LENGTH, &maxLength);

			maxLength = glm::max(maxLength, 512);
			std::vector<GLchar> infoLog(maxLength);
			
			glGetProgramInfoLog(rendererID, maxLength, &maxLength, &infoLog[0]);
			glDeleteProgram(rendererID);

			for (auto id : shaderIds)
				glDeleteShader(id);

			CORE_ERROR("GLSL linking error:\n{0}", infoLog.data());
			CORE_ASSERT(false, "Could not link shader program");
			return;
		}

		for (auto id : shaderIds) {
			glDetachShader(rendererID, id);
			glDeleteShader(id);
		}
		m_rendererID = rendererID;
	}

	void OpenGLShader::bind() const {
		glUseProgram(m_rendererID);
	}

	void OpenGLShader::unbind() const {
		glUseProgram(0);
	}

	void OpenGLShader::Dispatch(const glm::uvec3& size, uint32_t usage) {
		glUseProgram(m_rendererID);
		glDispatchCompute(size.x, size.y, size.z);
		if (usage != ComputeUsage::None)
			glMemoryBarrier(usage);
	}

	void OpenGLShader::setFloat(const std::string& name, float value) {
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1f(loc, value);
	}

	void OpenGLShader::setInt(const std::string& name, int value) {
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str()); 
		glUniform1i(loc, value);
	}

	void OpenGLShader::setVec(const std::string& name, const glm::vec4& value) {
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4f(loc, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::setMat(const std::string& name, const glm::mat4& value) {
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::setFloatArray(const std::string& name, const float* floats, uint32_t count) {
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1fv(loc, count, floats);
	}

	void OpenGLShader::setIntArray(const std::string& name, const int* ints, uint32_t count) {
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1iv(loc, count, ints);
	}

	void OpenGLShader::setVecArray(const std::string& name, const glm::vec4* vecs, uint32_t count) {
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4fv(loc, count, glm::value_ptr(*vecs));
	}

	void OpenGLShader::setMatArray(const std::string& name, const glm::mat4* mats, uint32_t count) {
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(loc, count, GL_FALSE, glm::value_ptr(*mats));
	}

	void OpenGLShader::setTexture(const std::string& name, uint32_t binding, const Ref<Texture>& texture, uint32_t mipLevel, AccesMode mode) {
		setTextureInternal(name, binding, texture->getNativeRendererObject(), mipLevel, mode, texture->getSettings().format);
	}
	void OpenGLShader::setTextureInternal(const std::string& name, uint32_t binding, void* texture, uint32_t mipLevel, AccesMode mode, TextureFormat format) {
		uint32_t textureID = *static_cast<uint32_t*>(texture);
		int32_t internalFormat;

		if (format == TextureFormat::Auto)
			glGetTextureLevelParameteriv(textureID, mipLevel, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

		else
			internalFormat = TextureFormatToGLenum(format).first;

		glUseProgram(m_rendererID);
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1i(loc, binding);
		glBindImageTexture(binding, textureID, mipLevel, GL_FALSE, 0, (uint32_t)mode, (uint32_t)internalFormat);
	}
}
