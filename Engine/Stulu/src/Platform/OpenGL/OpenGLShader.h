#pragma once
#include "Stulu/Renderer/Shader.h"
#include "Stulu/Renderer/Renderer.h"
typedef unsigned int GLenum;

namespace Stulu {
	class STULU_API OpenGLShaderCompiler : public ShaderCompiler {
	public:
		OpenGLShaderCompiler();

		virtual bool Compile(const ShaderSource& sources, ShaderCompileResult& result) const override;
		virtual bool CompileToCache(const ShaderSource& sources, const std::string& cacheFile, ShaderCompileResult& result) const override;

		virtual bool LoadFromCache(const std::string& cacheFile, ShaderCompileResult& result) const override;

		virtual bool isCacheUpToDate(const std::string& cacheFile, const std::string& shaderSourceFile) const override;

		virtual inline void AddHeader(const std::string& headers) override {
			m_headers.push_back(headers);
		}
		virtual inline void AddHeaderFront(const std::string& headers) override {
			m_headers.insert(m_headers.begin(), headers);
		}
		virtual inline const std::vector<std::string>& GetHeaders() const { return m_headers; }
		virtual inline std::vector<std::string>& GetHeaders() override { return m_headers; }
	private:
		std::vector<std::string> m_headers;

		std::string ApplyHeaders(const std::string& src) const;
	};

	class STULU_API OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name, const ShaderCompileResult& sources);
		virtual ~OpenGLShader();

		virtual void reload(const ShaderCompileResult& sources) override;

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void Dispatch(const glm::uvec3& numWorkGroups = { 1u,1u,1u }, uint32_t usage = 0xFFFFFFFF) override;
		virtual void Dispatch(const uint32_t numWorkGroupsX, const uint32_t numWorkGroupsY, const uint32_t numWorkGroupsZ, uint32_t usage = 0xFFFFFFFF) override {
			Dispatch({ numWorkGroupsX ,numWorkGroupsY,numWorkGroupsZ }, usage);
		}

		virtual void setFloat(const std::string& name, float value) override;
		virtual void setInt(const std::string& name, int value) override;
		virtual void setVec(const std::string& name, const glm::vec4& value) override;
		virtual void setMat(const std::string& name, const glm::mat4& value) override;
		virtual void setFloatArray(const std::string& name, const float* floats, uint32_t count) override;
		virtual void setIntArray(const std::string& name, const int* ints, uint32_t count) override;
		virtual void setVecArray(const std::string& name, const glm::vec4* vecs, uint32_t count) override;
		virtual void setMatArray(const std::string& name, const glm::mat4* mats, uint32_t count) override;

		virtual void setTexture(const std::string& name, uint32_t binding, const Ref<Texture>& texture, uint32_t mipLevel = 0, AccesMode mode = AccesMode::ReadWrite) override;
		virtual void setTextureInternal(const std::string& name, uint32_t binding, void* texture, uint32_t mipLevel = 0, AccesMode mode = AccesMode::ReadWrite, TextureFormat format = TextureFormat::Auto) override;

		virtual const std::string& getName() const override { return m_name; }
		virtual const void* getNativeRendererObject() const override { return &m_rendererID; };
	private:
		uint32_t m_rendererID;
		std::string m_name;

		void link(const ShaderCompileResult& sources);
	};
}

