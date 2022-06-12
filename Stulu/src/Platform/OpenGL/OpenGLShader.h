#pragma once
#include "Stulu/Renderer/Shader.h"

typedef unsigned int GLenum;

namespace Stulu {
	class STULU_API OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& path);
		OpenGLShader(const std::string& name, const std::string& src);
		OpenGLShader(const std::string& name, const std::string& vertex, const std::string& fragment);
		virtual ~OpenGLShader();

		virtual void reload(const std::string& path) override;

		virtual void bind() const override;
		virtual void unbind() const override;
		virtual const std::string& getName() const override { return m_name; }
		virtual const std::string& getSource(bool afterPreProcessing = true) const override { return afterPreProcessing ? m_sourcePreProcessed : m_source; }
		virtual void setMat4(const std::string& name, const glm::mat4& mat) override;
		virtual void setFloat4(const std::string& name, const glm::vec4& vec) override;
		virtual void setFloat3(const std::string& name, const glm::vec3& vec) override;
		virtual void setFloat2(const std::string& name, const glm::vec2& vec) override;
		virtual void setFloat(const std::string& name, const float Float) override;
		virtual void setInt(const std::string& name, const int32_t Int) override;
		virtual void setIntArray(const std::string& name, const int* values, uint32_t count) override;
		virtual void uploadMat4Uniform(const std::string& name,const glm::mat4& matrix);
		virtual void uploadMat3Uniform(const std::string& name,const glm::mat3& matrix);
		virtual void uploadFloat4Uniform(const std::string& name, const glm::vec4& float4);
		virtual void uploadFloat3Uniform(const std::string& name, const glm::vec3& float3);
		virtual void uploadFloat2Uniform(const std::string& name, const glm::vec2& float2);
		virtual void uploadFloatUniform(const std::string& name, const float _float);
		virtual void uploadIntUniform(const std::string& name, const int32_t _int);
		virtual void uploadIntArrayUniform(const std::string& name, const int* values, uint32_t count);

		virtual std::unordered_map<std::string, Ref<ShaderProperity>> getProperitys() override { return m_properitys; }
		virtual Ref<ShaderProperity> getProperity(std::string properityName) override { return m_properitys[properityName]; }
		virtual bool hasProperity(std::string properityName) override { return m_properitys.find(properityName) != m_properitys.end(); }
	private:
		uint32_t m_rendererID;
		std::string m_name;
		std::string m_source = "";
		std::string m_sourcePreProcessed = "";
		std::unordered_map<std::string, Ref<ShaderProperity>> m_properitys;

		std::string readFile(const std::string& path);
		std::unordered_map<GLenum,std::string> preProcess(const std::string src);
		void compile(const std::unordered_map<unsigned int, std::string>& shaderSrcs);
	};
}

