#include "st_pch.h"
#include "OpenGLComputeShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <Platform/OpenGL/OpenGLTexture.h>

namespace Stulu {
	OpenGLComputeShader::OpenGLComputeShader(const std::string& name, const std::string& src) {
		ST_PROFILING_FUNCTION();
		m_source = src;
		m_name = name;

		compile(src);
	}

	OpenGLComputeShader::~OpenGLComputeShader() {
		ST_PROFILING_FUNCTION();
		glDeleteProgram(m_rendererID);
	}

	void OpenGLComputeShader::reload(const std::string& src) {
		ST_PROFILING_FUNCTION();
		glDeleteProgram(m_rendererID);

		m_source = src;
		compile(src);
	}

	void OpenGLComputeShader::Dispatch(const glm::uvec3& size, uint32_t usage) {
		ST_PROFILING_FUNCTION();
		glUseProgram(m_rendererID);
		glDispatchCompute(size.x, size.y, size.z);
		if(usage != ComputeShader::Usage::None)
			glMemoryBarrier(usage);
	}

	void OpenGLComputeShader::compile(const std::string& src) {
		ST_PROFILING_FUNCTION();
		GLint program = glCreateProgram();
		GLuint shader = glCreateShader(GL_COMPUTE_SHADER);

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
			CORE_ERROR("GLSL Compute Shader compilation error:\n{0}", infoLog.data());
			CORE_ASSERT(false, "Compute Shader compilation of Shader \"" + m_name + "\" failed");
			return;
		}
		glAttachShader(program, shader);
		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			glDeleteProgram(program);
			glDeleteShader(shader);

			CORE_ERROR("GLSL compilation error:\n{0}", infoLog.data());
			CORE_ASSERT(false, "Could not link shader program");
			return;
		}

		glDetachShader(program, shader);
		glDeleteShader(shader);

		m_rendererID = program;
	}
	void OpenGLComputeShader::setTexture(const std::string& name, uint32_t binding, const Ref<Texture>& texture, uint32_t mipLevel, AccesMode mode) {
		setTextureInternal(name, binding, texture->getNativeRendererObject(), mipLevel, mode, (TextureSettings::Format)texture->getSettings().format);
	}
	void OpenGLComputeShader::setTextureInternal(const std::string& name, uint32_t binding, void* texture, uint32_t mipLevel, AccesMode mode, TextureSettings::Format format) {
		ST_PROFILING_FUNCTION();

		uint32_t textureID = *static_cast<uint32_t*>(texture);
		int32_t internalFormat;

		if (format == TextureSettings::Format::Auto)
			glGetTextureLevelParameteriv(textureID, mipLevel, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
		
		else
			internalFormat = TextureFormatToGLenum(format).first;

		glUseProgram(m_rendererID);
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1i(loc, binding);
		glBindImageTexture(binding, textureID, mipLevel, GL_FALSE, 0, (uint32_t)mode, (uint32_t)internalFormat);
	}
	void OpenGLComputeShader::setFloat(const std::string& name, float value) {
		ST_PROFILING_FUNCTION();
		glUseProgram(m_rendererID);
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1f(loc, value);
	}

	void OpenGLComputeShader::setInt(const std::string& name, int value) {
		ST_PROFILING_FUNCTION();
		glUseProgram(m_rendererID);
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1i(loc, value);
	}

	void OpenGLComputeShader::setVec(const std::string& name, const glm::vec4& value) {
		ST_PROFILING_FUNCTION();
		glUseProgram(m_rendererID);
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4f(loc, value.x, value.y, value.z, value.w);
	}

	void OpenGLComputeShader::setMat(const std::string& name, const glm::mat4& value) {
		ST_PROFILING_FUNCTION();
		glUseProgram(m_rendererID);
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLComputeShader::setFloatArray(const std::string& name, const float* floats, uint32_t count) {
		ST_PROFILING_FUNCTION();
		glUseProgram(m_rendererID);
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1fv(loc, count, floats);
	}

	void OpenGLComputeShader::setIntArray(const std::string& name, const int* ints, uint32_t count) {
		ST_PROFILING_FUNCTION();
		glUseProgram(m_rendererID);
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1iv(loc, count, ints);
	}

	void OpenGLComputeShader::setVecArray(const std::string& name, const glm::vec4* vecs, uint32_t count) {
		ST_PROFILING_FUNCTION();
		glUseProgram(m_rendererID);
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4fv(loc, count, glm::value_ptr(*vecs));
	}

	void OpenGLComputeShader::setMatArray(const std::string& name, const glm::mat4* mats, uint32_t count) {
		ST_PROFILING_FUNCTION();
		glUseProgram(m_rendererID);
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(loc, count, GL_FALSE, glm::value_ptr(*mats));
	}
}
