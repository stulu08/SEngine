#include "st_pch.h"
#include "OpenGLShader.h"
#include <glad/glad.h>

namespace Stulu {
	OpenGLShader::OpenGLShader(const std::string& vertex, const std::string& fragment) {
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		const GLchar* source = vertex.c_str();
		glShaderSource(vertexShader, 1, &source, 0);
		glCompileShader(vertexShader);
		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
			glDeleteShader(vertexShader);

			CORE_ASSERT(false, "Vertex Shader compilation failed");
			CORE_ERROR("{0}", infoLog.data());
			return;
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		source = fragment.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
			glDeleteShader(fragmentShader);

			CORE_ASSERT(false, "Fragment Shader compilation failed");
			CORE_ERROR("{0}", infoLog.data());
			return;
		}

		m_rendererID = glCreateProgram();

		glAttachShader(m_rendererID, vertexShader);
		glAttachShader(m_rendererID, fragmentShader);
		glLinkProgram(m_rendererID);

		GLint isLinked = 0;
		glGetProgramiv(m_rendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(m_rendererID, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_rendererID, maxLength, &maxLength, &infoLog[0]);
			glDeleteProgram(m_rendererID);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			CORE_ASSERT(false, "Could not link shader program");
			CORE_ERROR("{0}", infoLog.data());
			return;
		}

		glDetachShader(m_rendererID, vertexShader);
		glDetachShader(m_rendererID, fragmentShader);
	}

	OpenGLShader::~OpenGLShader() {
		glDeleteProgram(m_rendererID);
	}

	void OpenGLShader::bind() const {
		glUseProgram(m_rendererID);
	}
	void OpenGLShader::unbind() const {
		glUseProgram(0);
	}
}
