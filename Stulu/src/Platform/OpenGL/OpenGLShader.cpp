#include "st_pch.h"
#include "OpenGLShader.h"

#include <fstream>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include "Stulu/Scene/Resources.h"

namespace Stulu {
	static GLenum shaderTypeFromString(const std::string& type) {
		if (type == "vertex" || type == "vert")
			return GL_VERTEX_SHADER;
		else if (type == "fragment" || type == "frag" || type == "pixel")
			return GL_FRAGMENT_SHADER;
		CORE_ASSERT(false, "Unknown Shadertype: {0}", type);
		return 0;
	}
	static std::string stringFromShaderType(GLenum type) {
		switch (type) {
		case GL_VERTEX_SHADER:
			return "vertex";
			break;
		case GL_FRAGMENT_SHADER:
			return "fragment";
			break;
		}
		CORE_ASSERT(false, "Unknown Shadertype: {0}", type);
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& path)
	{
		ST_PROFILING_FUNCTION();
		size_t lastS = path.find_last_of("/\\");
		lastS = lastS == std::string::npos ? 0 : lastS + 1;
		size_t lastD = path.rfind('.');
		m_name = path.substr(lastS, lastD == std::string::npos ? path.size() - lastS : lastD - lastS);
		std::string shaderFile = readFile(path);
		auto sources = preProcess(shaderFile);
		compile(sources);
	}
	OpenGLShader::OpenGLShader(const std::string& name, const std::string& src) {
		ST_PROFILING_FUNCTION();
		m_name = name;
		auto sources = preProcess(src);
		compile(sources);
	}
	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertex, const std::string& fragment)
		: m_name(name) {
		ST_PROFILING_FUNCTION();
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertex;
		sources[GL_FRAGMENT_SHADER] = fragment;
		compile(sources);
	}

	std::string OpenGLShader::readFile(const std::string& path){
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
	std::unordered_map<GLenum, std::string> OpenGLShader::preProcess(const std::string _src) {
		ST_PROFILING_FUNCTION();
		std::unordered_map<GLenum, std::string> shaderSources;
		std::string shaderSrc = _src;
		{
			for (auto& p : s_preProcessorAdds) {
				std::string token = p.first;
				size_t pos = shaderSrc.find(token);
				if (pos != std::string::npos) {
					shaderSrc.replace(pos, token.length(), p.second);
				}
			}
		}
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

		return shaderSources;
	}
	void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSrcs){

		ST_PROFILING_FUNCTION();
		GLuint rendererID = glCreateProgram();
		CORE_ASSERT(shaderSrcs.size() <= 2,"Only 2 shaders are currently supported");
		int32_t shaderIndex = 0;
		std::array<GLenum, 2> shaderIds;
		for (auto& kv : shaderSrcs) {
			GLenum type = kv.first;
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
				CORE_ERROR("GLSL compilation error:\n{0}", infoLog.data());
				CORE_ASSERT(false, stringFromShaderType(type) + " Shader compilation of Shader \"" + m_name + "\" failed");
				break;
			}
			glAttachShader(rendererID, shader);
			shaderIds[shaderIndex++] = shader;
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
	OpenGLShader::~OpenGLShader() {
		ST_PROFILING_FUNCTION();
		glDeleteProgram(m_rendererID);
	}

	void OpenGLShader::bind() const {
		ST_PROFILING_FUNCTION();
		glUseProgram(m_rendererID);
	}
	void OpenGLShader::unbind() const {
		ST_PROFILING_FUNCTION();
		glUseProgram(0);
	}
	void OpenGLShader::setMat4(const std::string& name, const glm::mat4& mat){
		uploadMat4Uniform(name, mat);
	}
	void OpenGLShader::setFloat4(const std::string& name, const glm::vec4& vec) {
		uploadFloat4Uniform(name, vec);
	}
	void OpenGLShader::setFloat3(const std::string& name, const glm::vec3& vec) {
		uploadFloat3Uniform(name, vec);
	}
	void OpenGLShader::setFloat2(const std::string& name, const glm::vec2& vec) {
		uploadFloat2Uniform(name, vec);
	}
	void OpenGLShader::setFloat(const std::string& name, const float Float) {
		uploadFloatUniform(name, Float);
	}
	void OpenGLShader::setInt(const std::string& name, const int32_t Int) {
		uploadIntUniform(name, Int);
	}
	void OpenGLShader::setIntArray(const std::string& name, const int* values, uint32_t count) {
		uploadIntArrayUniform(name, values, count);
	}
	void OpenGLShader::uploadMat4Uniform(const std::string& name,const glm::mat4& matrix) {
		ST_PROFILING_FUNCTION();
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::uploadMat3Uniform(const std::string& name, const glm::mat3& matrix) {
		ST_PROFILING_FUNCTION();
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::uploadFloat4Uniform(const std::string& name, const glm::vec4& float4) {
		ST_PROFILING_FUNCTION();
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4f(loc, float4.x, float4.y, float4.z, float4.w);
	}
	void OpenGLShader::uploadFloat3Uniform(const std::string& name, const glm::vec3& float3) {
		ST_PROFILING_FUNCTION();
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform3f(loc, float3.x, float3.y, float3.z);
	}
	void OpenGLShader::uploadFloat2Uniform(const std::string& name, const glm::vec2& float2) {
		ST_PROFILING_FUNCTION();
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform2f(loc, float2.x, float2.y);
	}
	void OpenGLShader::uploadIntUniform(const std::string& name, const int32_t _int) {
		ST_PROFILING_FUNCTION();
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1i(loc, _int);
	}
	void OpenGLShader::uploadIntArrayUniform(const std::string& name, const int* values, uint32_t count) {
		ST_PROFILING_FUNCTION();
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1iv(loc, count, values);
	}
	void OpenGLShader::uploadFloatUniform(const std::string& name, const float _float) {
		ST_PROFILING_FUNCTION();
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1f(loc, _float);
	}
}
