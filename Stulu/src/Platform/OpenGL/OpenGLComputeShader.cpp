#include "st_pch.h"
#include "OpenGLComputeShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <Platform/OpenGL/OpenGLTexture.h>

namespace Stulu {
	OpenGLComputeShader::OpenGLComputeShader(const std::string& name, const std::string& src) {
		m_source = src;
		m_name = name;

		compile(src);
	}

	OpenGLComputeShader::~OpenGLComputeShader() {
		glDeleteProgram(m_rendererID);
	}

	void OpenGLComputeShader::reload(const std::string& src) {
		glDeleteProgram(m_rendererID);

		m_source = src;
		compile(src);
	}

	void OpenGLComputeShader::Dispatch(const glm::uvec3& size, uint32_t usage) {
		glUseProgram(m_rendererID);
		glDispatchCompute(size.x, size.y, size.z);
		glMemoryBarrier(usage);
	}

	void OpenGLComputeShader::compile(const std::string& src) {
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
		TextureSettings::Format form = (TextureSettings::Format)texture->getSettings().format;
		std::pair<GLenum, GLenum> format = TextureFormatToGLenum(form);

		glUseProgram(m_rendererID);
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1i(loc, binding);
		glBindImageTexture(binding, (int)*texture, mipLevel, GL_FALSE, 0, GL_READ_WRITE, format.first);
	}
	void OpenGLComputeShader::setFloat(const std::string& name, float value) {

	}

	void OpenGLComputeShader::setInt(const std::string& name, int value) {

	}

	void OpenGLComputeShader::setVec(const std::string& name, const glm::vec4& value) {

	}

	void OpenGLComputeShader::setMat(const std::string& name, const glm::mat4& value) {

	}

	void OpenGLComputeShader::setFloatArray(const std::string& name, const float* floats) {

	}

	void OpenGLComputeShader::setIntArray(const std::string& name, const int* ints) {

	}

	void OpenGLComputeShader::setVecArray(const std::string& name, const glm::vec4* vecs) {

	}

	void OpenGLComputeShader::setMatArray(const std::string& name, const glm::mat4* mats) {

	}
	const glm::ivec3 OpenGLComputeShader::getMaxWorkGroupCount() const {
		glm::ivec3 count = {0,0,0};
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &count.x);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &count.y);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &count.z);
		return count;
	}
	const glm::ivec3 OpenGLComputeShader::getMaxWorkGroupSizes() const {
		glm::ivec3 size = { 0,0,0 };
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &size.x);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &size.y);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &size.z);
		return size;
	}
	const uint32_t OpenGLComputeShader::getMaxWorkGroupInvocationCount() const {
		int32_t invc = 0;
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &invc);
		return (uint32_t)invc;
	}
}
