#include "st_pch.h"
#include "OpenGLShader.h"

#include <fstream>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

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
		std::string sShaderFile = readFile(path);
		auto sources = preProcess(sShaderFile, isMultiFile(sShaderFile));
		compile(sources);

		size_t lastS = path.find_last_of("/\\");
		lastS = lastS == std::string::npos ? 0 : lastS + 1;
		size_t lastD = path.rfind('.');
		m_name = path.substr(lastS, lastD == std::string::npos ? path.size() - lastS : lastD - lastS);

	}
	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertex, const std::string& fragment)
		: m_name(name) {
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertex;
		sources[GL_FRAGMENT_SHADER] = fragment;
		compile(sources);
	}

	std::string OpenGLShader::readFile(const std::string& path){
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
	std::unordered_map<GLenum, std::string> OpenGLShader::preProcess(const std::string src, bool multiFile) {
		std::unordered_map<GLenum, std::string> shaderSources;
		if (multiFile) {
			const char* typeToken = "#include";
			size_t typeTokenLength = strlen(typeToken);
			size_t pos = src.find(typeToken, 0);
			while (pos != std::string::npos)
			{
				size_t eol = src.find_first_of("\r\n", pos);
				size_t typeEnder = src.find_first_of("|", pos);
				//CORE_ASSERT(eol != std::string::npos, "Syntax error");
				size_t begin = pos + typeTokenLength + 1;
				std::string type = src.substr(begin, typeEnder - begin);
				begin += type.length() + 1;
				std::string path = src.substr(begin, eol - begin);

				std::string shaderSrc = readFile(path);

				CORE_ASSERT(shaderTypeFromString(type), "Invalid shader type specified");

				size_t nextLinePos = src.find_first_not_of("\r\n", eol);
				pos = src.find(typeToken, nextLinePos);
				shaderSources[shaderTypeFromString(type)] = shaderSrc;
			}
			return shaderSources;
		}
		const char* typeToken = "##type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = src.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = src.find_first_of("\r\n", pos);
			CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = src.substr(begin, eol - begin);
			CORE_ASSERT(shaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = src.find_first_not_of("\r\n", eol);
			pos = src.find(typeToken, nextLinePos);
			shaderSources[shaderTypeFromString(type)] = src.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? src.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}
	void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSrcs){

		GLuint rendererID = glCreateProgram();
		CORE_ASSERT(shaderSrcs.size() <= 2,"Only 2 shaders are currently supported");
		int shaderIndex = 0;
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
				CORE_ASSERT(false, "{0} Shader compilation failed", stringFromShaderType(type));
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

		for (auto id : shaderIds)
			glDetachShader(rendererID, id);

		m_rendererID = rendererID;

	}
	bool OpenGLShader::isMultiFile(const std::string src) {
		return src.rfind("#multifile", 0) == 0;
	}
	OpenGLShader::~OpenGLShader() {
		glDeleteProgram(m_rendererID);
	}

	void OpenGLShader::bind() const {
		int32_t active_id = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &active_id);
		if(active_id != m_rendererID)
			glUseProgram(m_rendererID);
	}
	void OpenGLShader::unbind() const {
		glUseProgram(0);
	}
	void OpenGLShader::uploadMat4Uniform(const std::string& name,const glm::mat4& matrix)
	{
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::uploadMat3Uniform(const std::string& name, const glm::mat3& matrix)
	{
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::uploadFloat4Uniform(const std::string& name, const glm::vec4& float4)
	{
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4f(loc, float4.x, float4.y, float4.z, float4.w);
	}
	void OpenGLShader::uploadFloat3Uniform(const std::string& name, const glm::vec3& float3)
	{
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform3f(loc, float3.x, float3.y, float3.z);
	}
	void OpenGLShader::uploadFloat2Uniform(const std::string& name, const glm::vec2& float2)
	{
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform2f(loc, float2.x, float2.y);
	}
	void OpenGLShader::uploadIntUniform(const std::string& name, const int _int)
	{
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1i(loc, _int);
	}
	void OpenGLShader::uploadFloatUniform(const std::string& name, const float _float)
	{
		GLint loc = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1f(loc, _float);
	}
}
