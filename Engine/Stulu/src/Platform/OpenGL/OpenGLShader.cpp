#include "st_pch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "Stulu/Core/Resources.h"
#include "Stulu/Renderer/Shader.h"
#include "OpenGLTexture.h"

namespace Stulu {
	static std::string stringFromShaderType(GLenum type) {
		switch (type) {
		case GL_VERTEX_SHADER:
			return "Vertex";
			break;
		case GL_FRAGMENT_SHADER:
			return "Fragment";
			break;
		case GL_COMPUTE_SHADER:
			return "Compute";
			break;
		}
		CORE_ERROR("Unknown Shadertype: {0}", type);
		return "";
	}
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
	OpenGLShader::OpenGLShader(const std::string& name, const ShaderSource& sources)
		: m_name(name) {
		ST_PROFILING_FUNCTION();
		compile(sources);
	}

	OpenGLShader::~OpenGLShader() {
		ST_PROFILING_FUNCTION();
		glDeleteProgram(m_rendererID);
	}

	void OpenGLShader::reload(const ShaderSource& sources) {
		ST_PROFILING_FUNCTION();

		glDeleteProgram(m_rendererID);
		m_rendererID = 0;

		compile(sources);
	}
	void OpenGLShader::compile(const ShaderSource& sources){
		ST_PROFILING_FUNCTION();

		GLuint rendererID = glCreateProgram();
		
		std::vector<GLenum> shaderIds;
		shaderIds.reserve(sources.Size());

		for (uint32_t i = 0; i < sources.Size(); i++) {
			const auto& kv = sources.Get(i);

			GLenum type = shaderTypeToGl(kv.first);
			const std::string& src = kv.second;

			GLuint shader = glCreateShader(type);
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
				CORE_ERROR("GLSL {1} Shader compilation error:\n{0}", infoLog.data(), stringFromShaderType(type));
				CORE_ASSERT(false, stringFromShaderType(type) + " Shader compilation of \"" + m_name + "\" failed");
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
			for(auto id : shaderIds)
				glDeleteShader(id);

			CORE_ERROR("GLSL compilation error:\n{0}", infoLog.data());
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
		ST_PROFILING_FUNCTION();
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
